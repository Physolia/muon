/***************************************************************************
 *   Copyright © 2011-2013 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#ifndef OAUTHSESSION_H
#define OAUTHSESSION_H

#include <QObject>

// Qt includes
#include <QPair>
#include <QVariantMap>

// Own includes
#include "ReviewsBackend/AbstractReviewsBackend.h"
#include "OAuthPost.h"

namespace KIO {
    class StoredTransferJob;
}

namespace QOAuth {
    class Interface;
}

class UbuntuLoginBackend;
class OAuthPost;
class KJob;

/**
 * The OAuthSession class holds information about an OAuth login session.
 * It also handles dispatching POST requests requiring OAuth validation.
 */
class OAuthSession : public QObject
{
    Q_OBJECT
public:
    explicit OAuthSession(QObject *parent = nullptr);
    static OAuthSession* global();

    UbuntuLoginBackend *loginBackend() const;

private:
    UbuntuLoginBackend* m_loginBackend;
    QOAuth::Interface* m_oauthInterface;
    QList<OAuthPost> m_pendingRequests;

    static OAuthSession* s_self;

signals:
    void loginStateChanged();

public slots:
    KIO::StoredTransferJob *postInformation(const OAuthPost &info);
    KIO::StoredTransferJob *getInformation(const OAuthPost &info);
    void refreshConsumerKeys();
    void updateCredentials(const QMap<QString, QVariant> &creds);

private slots:
    void informationPosted(KJob* job);
};

#endif // OAUTHSESSION_H
