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

#ifndef APPLICATIONVIEWWIDGET_H
#define APPLICATIONVIEWWIDGET_H

#include <QtCore/QPair>

#include <LibQApt/Package>

#include "AbstractViewBase.h"

class QCheckBox;
class QIcon;
class QLabel;
class QTreeView;

class KComboBox;

class Application;
class ApplicationBackend;
class ApplicationDelegate;
class ApplicationDetailsView;
class ApplicationModel;
class ApplicationProxyModel;
class Category;

namespace QApt {
    class Backend;
}

class ApplicationViewWidget : public AbstractViewBase
{
    Q_OBJECT
public:
    ApplicationViewWidget(QWidget *parent, ApplicationBackend *appBackend);
    ~ApplicationViewWidget();

    virtual void search(const QString &text);

private:
    QApt::Backend *m_backend;
    ApplicationBackend *m_appBackend;
    ApplicationModel *m_appModel;
    ApplicationProxyModel *m_proxyModel;
    QPair<AbstractViewBase *, Application *> m_currentPair;
    bool m_canShowTechnical;

    QLabel *m_headerIcon;
    QLabel *m_headerLabel;
    QCheckBox *m_techCheckBox;
    KComboBox *m_sortCombo;
    QTreeView *m_treeView;
    ApplicationDelegate *m_delegate;
    ApplicationDetailsView *m_detailsView;

private Q_SLOTS:
    void infoButtonClicked(Application *app);
    void onSubViewDestroyed();
    void sortComboChanged(int index);
    void updateSortCombo();
    void techCheckChanged(int state);

public Q_SLOTS:
    void setBackend(QApt::Backend *backend);

    void setTitle(const QString &title);
    void setIcon(const QIcon &icon);
    void setStateFilter(QApt::Package::State state);
    void setOriginFilter(const QString &origin);
    void setFiltersFromCategory(Category *category);
    void setShouldShowTechnical(bool show);
    void setCanShowTechnical(bool canShow);

Q_SIGNALS:
    void switchToSubView(AbstractViewBase *view);
    void registerNewSubView(AbstractViewBase *view);
};

#endif