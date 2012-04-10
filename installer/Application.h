/***************************************************************************
 *   Copyright © 2010-2011 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include <KUrl>

#include <LibQApt/Package>

namespace QApt {
    class Backend;
}

class Application
{
public:
    explicit Application(const QString &fileName, QApt::Backend *backend);
    explicit Application(QApt::Package *package, QApt::Backend *backend);
    ~Application();

    QString name();
    QString untranslatedName();
    QString comment();
    QApt::Package *package();
    QString icon() const;
    QString mimetypes() const;
    QString menuPath();
    QString categories();
    KUrl screenshotUrl(QApt::ScreenshotType type);
    QApt::PackageList addons();
    bool isValid() const;
    bool isTechnical() const;

    QByteArray getField(const QByteArray &field) const;
    QHash<QByteArray, QByteArray> desktopContents();

private:
    QString m_fileName;
    QHash<QByteArray, QByteArray> m_data;
    QApt::Backend *m_backend;
    QApt::Package *m_package;

    bool m_isValid;
    bool m_isTechnical;

    QVector<QPair<QString, QString> > locateApplication(const QString &_relPath, const QString &menuId) const;
};

#endif