/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#include "ApplicationUpdates.h"
#include "BackendsSingleton.h"
#include <Application.h>
#include <ApplicationBackend.h>
#include "MuonInstallerMainWindow.h"
#include <LibQApt/Backend>
#include <QDebug>

ApplicationUpdates::ApplicationUpdates(QObject* parent): QObject(parent)
{
    QApt::Backend* backend = BackendsSingleton::self()->backend();
    connect(backend, SIGNAL(errorOccurred(QApt::ErrorCode,QVariantMap)), BackendsSingleton::self()->mainWindow(),
            SLOT(errorOccurred(QApt::ErrorCode,QVariantMap)));
    connect(backend, SIGNAL(commitProgress(QString,int)),
            SIGNAL(progress(QString,int)));
    connect(backend, SIGNAL(downloadMessage(int,QString)), SIGNAL(downloadMessage(int,QString)));
    connect(backend, SIGNAL(debInstallMessage(QString)), SIGNAL(installMessage(QString)));
    connect(backend, SIGNAL(workerEvent(QApt::WorkerEvent)),
            this, SLOT(workerEvent(QApt::WorkerEvent)));
}

void ApplicationUpdates::updateApplications(const QList< QObject* >& apps)
{
    foreach(QObject* app, apps) {
        qobject_cast<Application*>(app)->package()->setInstall();
    }
    BackendsSingleton::self()->backend()->commitChanges();
}

void ApplicationUpdates::upgradeAll()
{
    QApt::Backend* backend = BackendsSingleton::self()->backend();
    QApt::PackageList packages = backend->upgradeablePackages();
    foreach(QApt::Package* p, packages) {
        p->setInstall();
    }
    backend->commitChanges();
}

void ApplicationUpdates::workerEvent(QApt::WorkerEvent e)
{
    if(e==QApt::CommitChangesFinished) {
        qDebug() << "updates done. Reloading...";
        
        //when it's done, trigger a reload of the whole system
        BackendsSingleton::self()->applicationBackend()->reload();
        emit updatesFinnished();
    }
}