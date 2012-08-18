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

#include "ResourcesModel.h"

#include <KGlobal>

#include "resources/AbstractResourcesBackend.h"
#include "AbstractResource.h"
#include <ReviewsBackend/Rating.h>
#include <ReviewsBackend/AbstractReviewsBackend.h>
#include <Transaction/Transaction.h>
#include <QDebug>

K_GLOBAL_STATIC_WITH_ARGS(ResourcesModel, globalResourcesModel, (0))

ResourcesModel *ResourcesModel::global()
{
    return globalResourcesModel;
}

ResourcesModel::ResourcesModel(QObject* parent)
    : QAbstractListModel(parent)
{
    QHash< int, QByteArray > roles = roleNames();
    roles[NameRole] = "name";
    roles[IconRole] = "icon";
    roles[CommentRole] = "comment";
    roles[ActionRole] = "action";
    roles[StateRole] = "state";
    roles[RatingRole] = "rating";
    roles[RatingPointsRole] = "ratingPoints";
    roles[SortableRatingRole] = "sortableRating";
    roles[ActiveRole] = "active";
    roles[ProgressRole] = "progress";
    roles[ProgressTextRole] = "progressText";
    roles[InstalledRole] = "isInstalled";
    roles[ApplicationRole] = "application";
    roles[PopConRole] = "popcon";
    roles[OriginRole] = "origin";
    roles[UntranslatedNameRole] = "untranslatedName";
    roles[CanUpgrade] = "canUpgrade";
    roles[PackageNameRole] = "packageName";
    roles[IsTechnicalRole] = "isTechnical";
    roles[CategoryRole] = "category";
    roles[SectionRole] = "section";
    roles[MimeTypes] = "mimetypes";
    setRoleNames(roles);
}

void ResourcesModel::addResourcesBackend(AbstractResourcesBackend* resources)
{
    Q_ASSERT(!m_backends.contains(resources));
    QVector<AbstractResource*> newResources = resources->allResources();
    int current = rowCount();
    beginInsertRows(QModelIndex(), current, current+newResources.size());
    m_backends += resources;
    m_resources.append(newResources);
    endInsertRows();
    
    connect(resources, SIGNAL(backendReady()), SLOT(resetCaller()));
    connect(resources, SIGNAL(reloadStarted()), SLOT(cleanCaller()));
    connect(resources, SIGNAL(reloadFinished()), SLOT(resetCaller()));
    connect(resources, SIGNAL(updatesCountChanged()), SIGNAL(updatesCountChanged()));
    connect(resources, SIGNAL(allDataChanged()), SLOT(updateCaller()));
    connect(resources, SIGNAL(searchInvalidated()), SIGNAL(searchInvalidated()));
    
    connect(resources, SIGNAL(transactionAdded(Transaction*)), SIGNAL(transactionAdded(Transaction*)));
    connect(resources, SIGNAL(transactionCancelled(Transaction*)), SIGNAL(transactionCancelled(Transaction*)));
    connect(resources, SIGNAL(transactionProgressed(Transaction*,int)), SIGNAL(transactionProgressed(Transaction*,int)));
    connect(resources, SIGNAL(transactionRemoved(Transaction*)), SIGNAL(transactionRemoved(Transaction*)));
    connect(resources, SIGNAL(transactionsEvent(TransactionStateTransition,Transaction*)), SIGNAL(transactionsEvent(TransactionStateTransition,Transaction*)));
    
    connect(this, SIGNAL(transactionAdded(Transaction*)), SLOT(transactionChanged(Transaction*)));
    connect(this, SIGNAL(transactionRemoved(Transaction*)), SLOT(transactionChanged(Transaction*)));
    connect(this, SIGNAL(transactionCancelled(Transaction*)), SLOT(transactionChanged(Transaction*)));

    emit backendsChanged();
}

AbstractResource* ResourcesModel::resourceAt(int row) const
{
    for (auto it = m_resources.constBegin(); it != m_resources.constEnd(); ++it) {
        if (it->size()<=row)
            row -= it->size();
        else
            return it->at(row);
    }
    return 0;
}

QModelIndex ResourcesModel::resourceIndex(AbstractResource* res) const
{
    AbstractResourcesBackend* backend = res->backend();
    int row = 0, backends = m_backends.count();
    for(int i=0; i<backends; i++) {
        if(m_backends[i]!=backend)
            row += m_resources[i].size();
        else {
            int pos = m_resources[i].indexOf(res);
            Q_ASSERT(pos>0);
            row += pos;
        }
    }

    Q_ASSERT(row);
    return index(row);
}

QVariant ResourcesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    AbstractResource* resource = resourceAt(index.row());
    switch(role) {
        case ActiveRole: {
            //TODO: Maybe move to AbstractResource?
            Transaction* t = resource->backend()->currentTransactionState().second;
            return t && t->resource() == resource;
        }
        case ApplicationRole:
            return qVariantFromValue<QObject*>(resource);
        case RatingPointsRole:
        case RatingRole:
        case SortableRatingRole:{
            Rating* rating = resource->backend()->reviewsBackend()->ratingForApplication(resource);
            return rating ? rating->property(roleNames().value(role)) : -1;
        }
        default: {
            QByteArray roleText = roleNames().value(role);
            if(roleText.isEmpty())
                return QVariant();
            else if(resource->metaObject()->indexOfProperty(roleText) < 0) {
                qDebug() << "unknown role:" << role << roleText;
                return QVariant();
            } else
                return resource->property(roleNames().value(role));
        }
    }
}

int ResourcesModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid()) return 0;
    int ret = 0;
    foreach(const QVector<AbstractResource*>& resources, m_resources) {
        ret += resources.size();
    }
    return ret;
}

void ResourcesModel::cleanCaller()
{
    AbstractResourcesBackend* backend = qobject_cast<AbstractResourcesBackend*>(sender());
    Q_ASSERT(backend);
    int pos = m_backends.indexOf(backend);
    Q_ASSERT(pos>=0);
    QVector<AbstractResource*>* backendsResources = &m_resources[pos];
    int before = 0;
    for(auto it = m_resources.constBegin();
        it != m_resources.constEnd() && it != backendsResources;
        ++it)
    {
        before+= it->size();
    }
    
    beginRemoveRows(QModelIndex(), before, before+backendsResources->count()-1);
    backendsResources->clear();
    endRemoveRows();
}

void ResourcesModel::resetCaller()
{
    AbstractResourcesBackend* backend = qobject_cast<AbstractResourcesBackend*>(sender());
    Q_ASSERT(backend);
    
    QVector< AbstractResource* > res = backend->allResources();

    if(!res.isEmpty()) {
        int pos = m_backends.indexOf(backend);
        Q_ASSERT(pos>=0);
        QVector<AbstractResource*>* backendsResources = &m_resources[pos];
        int before = 0;
        for(auto it=m_resources.constBegin();
            it != m_resources.constEnd() && it !=backendsResources;
            ++it)
        {
            before += it->size();
        }
        
        beginInsertRows(QModelIndex(), before, before+res.size()-1);
        *backendsResources = res;
        endInsertRows();
        emit updatesCountChanged();
    }
}

void ResourcesModel::updateCaller()
{
    AbstractResourcesBackend* backend = qobject_cast<AbstractResourcesBackend*>(sender());
    int pos = m_backends.indexOf(backend);
    QVector<AbstractResource*>* backendsResources = &m_resources[pos];
    int before = 0;
    for(QVector<QVector<AbstractResource*> >::const_iterator it=m_resources.constBegin();
        it!=m_resources.constEnd() && it!=backendsResources;
        ++it)
    {
        before+= it->size();
    }
    
    emit dataChanged(index(before), index(before+backendsResources->size()-1));
}

QVector< AbstractResourcesBackend* > ResourcesModel::backends() const
{
    return m_backends;
}

AbstractResource* ResourcesModel::resourceByPackageName(const QString& name)
{
    foreach(AbstractResourcesBackend* backend, m_backends)
    {
        AbstractResource* res = backend->resourceByPackageName(name);
        if(res) {
            return res;
        }
    }
    return 0;
}

int ResourcesModel::updatesCount() const
{
    int ret = 0;

    foreach(AbstractResourcesBackend* backend, m_backends) {
        ret += backend->updatesCount();
    }

    return ret;
}

void ResourcesModel::installApplication(AbstractResource* app)
{
    app->backend()->installApplication(app);
}

void ResourcesModel::installApplication(AbstractResource* app, const QHash< QString, bool >& state)
{
    app->backend()->installApplication(app, state);
}

void ResourcesModel::removeApplication(AbstractResource* app)
{
    app->backend()->removeApplication(app);
}

void ResourcesModel::cancelTransaction(AbstractResource* app)
{
    app->backend()->cancelTransaction(app);
}

void ResourcesModel::transactionChanged(Transaction* t)
{
    QModelIndex idx = resourceIndex(t->resource());
    dataChanged(idx, idx);
}