/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "ApplicationWindow.h"

// Qt includes
#include <QStandardItemModel>
#include <QtCore/QTimer>
#include <QtGui/QSplitter>
#include <QtGui/QStackedWidget>

// KDE includes
#include <KIcon>
#include <KDebug>

#include <LibQApt/Backend>

// Own includes
#include "ApplicationBackend.h"
#include "Application.h"
#include "AvailableView.h"
#include "ViewSwitcher.h"
#include "ApplicationModel/ApplicationView.h"

ApplicationWindow::ApplicationWindow()
    : MuonMainWindow()
    , m_powerInhibitor(0)
{
    initGUI();
    QTimer::singleShot(10, this, SLOT(initObject()));
}

ApplicationWindow::~ApplicationWindow()
{
}

void ApplicationWindow::initGUI()
{
    setWindowTitle(i18n("Muon Software Center"));

    m_mainWidget = new QSplitter(this);
    m_mainWidget->setOrientation(Qt::Horizontal);
    connect(m_mainWidget, SIGNAL(splitterMoved(int, int)), this, SLOT(saveSplitterSizes()));
    setCentralWidget(m_mainWidget);

    // Set up the navigational sidebar on the right
    m_viewSwitcher = new ViewSwitcher(this);
    connect(m_viewSwitcher, SIGNAL(activated(const QModelIndex &)),
           this, SLOT(changeView(const QModelIndex &)));
    m_mainWidget->addWidget(m_viewSwitcher);

    // Set up the main pane
    m_viewStack = new QStackedWidget(this);
    m_mainWidget->addWidget(m_viewStack);

    m_viewModel = new QStandardItemModel(this);
    m_viewSwitcher->setModel(m_viewModel);

    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            this, SLOT(populateViews()));
}

void ApplicationWindow::initObject()
{
    m_appBackend = new ApplicationBackend(this);
    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            m_appBackend, SLOT(setBackend(QApt::Backend *)));

    MuonMainWindow::initObject();
}

void ApplicationWindow::reload()
{
    m_appBackend->reload();
    populateViews();
}

void ApplicationWindow::populateViews()
{
    m_viewHash.clear();

    QStringList originLabels = m_backend->originLabels();
    QStringList originNames;
    foreach (const QString &originLabel, originLabels) {
        originNames << m_backend->origin(originLabel);
    }

    if (originNames.contains("Ubuntu")) {
        int index = originNames.indexOf("Ubuntu");
        originNames.move(index, 0); // Move to front of the list
    }

    if (originNames.contains("Canonical")) {
        int index = originNames.indexOf("Canonical");
        originNames.move(index, 1); // Move to 2nd spot
    }

    QStandardItem *parentItem = m_viewModel->invisibleRootItem();

    QStandardItem *availableItem = new QStandardItem;
    availableItem->setEditable(false);
    availableItem->setIcon(KIcon("applications-other").pixmap(32,32));
    availableItem->setText(i18nc("@item:inlistbox Parent item for available software", "Get Software"));
    availableItem->setData(CatView, ViewTypeRole);
    parentItem->appendRow(availableItem);
    m_viewHash[availableItem->index()] = 0;

    QStandardItem *installedItem = new QStandardItem;
    installedItem->setEditable(false);
    installedItem->setIcon(KIcon("computer"));
    installedItem->setText(i18nc("@item:inlistbox Parent item for installed software", "Installed Software"));
    installedItem->setData(AppView, ViewTypeRole);
    installedItem->setData(QApt::Package::Installed, StateFilterRole);
    parentItem->appendRow(installedItem);
    m_viewHash[installedItem->index()] = 0;

    parentItem = availableItem;
    foreach(const QString &originName, originNames) {
        QString originLabel = m_backend->originLabel(originName);
        QStandardItem *viewItem = new QStandardItem;
        viewItem->setEditable(false);
        viewItem->setText(originLabel);
        viewItem->setData(originName, OriginFilterRole);
        viewItem->setData(AppView, ViewTypeRole);

        if (originName == "Ubuntu") {
            viewItem->setText(i18n("Provided by Kubuntu"));
            viewItem->setIcon(KIcon("ubuntu-logo"));
        }

        if (originName == "Canonical") {
            viewItem->setText(i18n("Canonical Partners"));
            viewItem->setIcon(KIcon("partner"));
        }

        if (originName.startsWith(QLatin1String("LP-PPA"))) {
            viewItem->setIcon(KIcon("user-identity"));
        }

        availableItem->appendRow(viewItem);
        m_viewHash[viewItem->index()] = 0;
    }

    parentItem = installedItem;
    foreach(const QString & originName, originNames) {
        // We must spread the word of Origin. Hallowed are the Ori! ;P
        QString originLabel = m_backend->originLabel(originName);
        QStandardItem *viewItem = new QStandardItem;
        viewItem->setEditable(false);
        viewItem->setText(originLabel);
        viewItem->setData(QApt::Package::Installed, StateFilterRole);
        viewItem->setData(originName, OriginFilterRole);

        if (originName == "Ubuntu") {
            viewItem->setText("Kubuntu");
            viewItem->setIcon(KIcon("ubuntu-logo"));
            viewItem->setData(AppView, ViewTypeRole);
        }

        if (originName == "Canonical") {
            viewItem->setIcon(KIcon("partner"));
            viewItem->setData(AppView, ViewTypeRole);
        }

        if (originName.startsWith(QLatin1String("LP-PPA"))) {
            viewItem->setIcon(KIcon("user-identity"));
            viewItem->setData(AppView, ViewTypeRole);
        }

        installedItem->appendRow(viewItem);
        m_viewHash[viewItem->index()] = 0;
    }
}

void ApplicationWindow::changeView(const QModelIndex &index)
{
    QWidget *view = m_viewHash.value(index);

    // Create new widget if not already created
    if (!view) {
        switch (index.data(ViewTypeRole).toInt()) {
        case AppView: {
            QString originFilter = index.data(OriginFilterRole).toString();
            QApt::Package::State stateFilter = (QApt::Package::State)index.data(StateFilterRole).toInt();
            view = new ApplicationView(this, m_appBackend);
            ApplicationView *appView = static_cast<ApplicationView *>(view);
            m_viewStack->addWidget(view);
            appView->setBackend(m_backend);
            appView->setStateFilter(stateFilter);
            appView->setOriginFilter(originFilter);
        }
        break;
        case CatView:
            view = new AvailableView(this, m_appBackend);
            m_viewStack->addWidget(view);
            break;
        case HistoryView:
        case InvalidView:
        default:
            break;
        }
    }

    m_viewStack->setCurrentWidget(view);

    m_viewHash[index] = view;
}

ApplicationBackend *ApplicationWindow::appBackend() const
{
    return m_appBackend;
}

#include "ApplicationWindow.moc"
