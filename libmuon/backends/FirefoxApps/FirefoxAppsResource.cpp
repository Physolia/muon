/***************************************************************************
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
 *                                                                         *
 *   This program is free software {} you can redistribute it and/or       *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation {} either version 2 of      *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY {} without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "FirefoxAppsResource.h"

FirefoxAppsResource::FirefoxAppsResource(const QString& path, AbstractResourcesBackend* parent)
    : AbstractResource(parent)
    , m_desktop(path)
{}

QList<PackageState> FirefoxAppsResource::addonsInformation() { return QList<PackageState>(); }
QString FirefoxAppsResource::section() { return "webapps"; }
QString FirefoxAppsResource::origin() const { return "marketplace"; }
QString FirefoxAppsResource::longDescription() const { return m_desktop.readComment(); }
QString FirefoxAppsResource::availableVersion() const { return "1.0"; }
QString FirefoxAppsResource::installedVersion() const { return "1.0"; }
QString FirefoxAppsResource::license() { return "unknown"; }
int FirefoxAppsResource::downloadSize() { return 0; }
QUrl FirefoxAppsResource::screenshotUrl() { return QUrl(); }
QUrl FirefoxAppsResource::thumbnailUrl() { return QUrl(); }
QUrl FirefoxAppsResource::homepage() const { return QUrl(); }
QString FirefoxAppsResource::categories() { return "ffxwebapps"; }
AbstractResource::State FirefoxAppsResource::state() { return AbstractResource::Installed; }
QString FirefoxAppsResource::icon() const { return m_desktop.readIcon(); }
QString FirefoxAppsResource::comment() { return m_desktop.readComment(); }
QString FirefoxAppsResource::name() { return m_desktop.readName(); }
QString FirefoxAppsResource::packageName() const { return m_desktop.fileName(); }
void FirefoxAppsResource::fetchChangelog() { emit changelogFetched(QString()); }

