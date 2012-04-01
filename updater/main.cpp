/***************************************************************************
 *   Copyright © 2010-2012 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#include "MainWindow.h"

#include <KUniqueApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KStandardDirs>

#include <stdio.h>

static const char description[] =
    I18N_NOOP("An update manager");

static const char version[] = "1.3.1 \"Daring Dalek\"";

int main(int argc, char **argv)
{
    KAboutData about("muon-updater", "muon-updater", ki18n("Muon Update Manager"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("©2010, 2011 Jonathan Thomas"), KLocalizedString(), 0);
    about.addAuthor(ki18n("Jonathan Thomas"), KLocalizedString(), "echidnaman@kubuntu.org");
    about.setProgramIconName("system-software-update");
    about.setProductName("muon/updater");

    KCmdLineArgs::init(argc, argv, &about);

    if (!KUniqueApplication::start()) {
        fprintf(stderr, "Update Manager is already running!\n");
        return 0;
    }

    KUniqueApplication app;
    app.disableSessionManagement();
    // Translations
    KGlobal::locale()->insertCatalog("app-install-data");
    KGlobal::locale()->insertCatalog("libmuon");
    // Needed for KIcon compatibility w/ application icons from app-install-data
    KGlobal::dirs()->addResourceDir("appicon", "/usr/share/app-install/icons/");

    MainWindow *mainWindow = new MainWindow;
    mainWindow->show();

    return app.exec();
}
