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

#include "PackageProxyModel.h"

// KDE includes
#include <KLocale>

// LibQApt includes
#include <libqapt/backend.h>

// Own includes
#include "PackageModel.h"

PackageProxyModel::PackageProxyModel(QObject *parent)
        : QSortFilterProxyModel(parent)
        , m_backend(0)
        , m_packages(QApt::PackageList())
        , m_searchText(QString())
        , m_groupFilter(QString())
        , m_stateFilter((QApt::Package::PackageState)0)
        , m_sortByRelevancy(false)
{
}

PackageProxyModel::~PackageProxyModel()
{
}

void PackageProxyModel::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    m_packages = static_cast<PackageModel*>(sourceModel())->packages();
}

void PackageProxyModel::search(const QString &searchText)
{
    // 1-character searches are painfully slow. >= 2 chars are fine, though
    m_packages.clear();
    if (searchText.size() > 1) {
        m_packages = m_backend->search(searchText);
        m_sortByRelevancy = true;
    } else {
        m_packages =  static_cast<PackageModel*>(sourceModel())->packages();
        m_sortByRelevancy = false;
    }
    invalidate();
}

void PackageProxyModel::setGroupFilter(const QString &filterText)
{
    m_groupFilter = filterText;
    invalidateFilter();
}

void PackageProxyModel::setStateFilter(QApt::Package::PackageState state)
{
    m_stateFilter = state;
    invalidateFilter();
}

bool PackageProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //Our "main"-method
    QApt::Package *package = static_cast<PackageModel*>(sourceModel())->packageAt(sourceModel()->index(sourceRow, 1, sourceParent));
    //We have a package as internal pointer
    if (!package) {
        return false;
    }

    if (!m_groupFilter.isEmpty()) {
        if (!package->section().contains(m_groupFilter)) {
            return false;
        }
    }

    if (!m_stateFilter == 0) {
        if ((bool)(package->state() & m_stateFilter) == false) {
            return false;
        }
    }

    if (m_sortByRelevancy) {
        return m_packages.contains(package);
    }
    return true;
}

QApt::Package *PackageProxyModel::packageAt(const QModelIndex &index) const
{
    // Since our representation is almost bound to change, we need to grab the parent model's index
    QModelIndex sourceIndex = mapToSource(index);
    QApt::Package *package = static_cast<PackageModel*>(sourceModel())->packageAt(sourceIndex);
    return package;
}

void PackageProxyModel::reset()
{
    beginRemoveRows(QModelIndex(), 0, m_packages.size());
    m_packages =  static_cast<PackageModel*>(sourceModel())->packages();
    endRemoveRows();
    invalidate();
}

bool PackageProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (m_sortByRelevancy) {
            // This is expensive for very large datasets. It takes about 3 seconds with 30,000 packages
            QApt::Package *leftPackage = static_cast<PackageModel*>(sourceModel())->packageAt(left);
            QApt::Package *rightPackage = static_cast<PackageModel*>(sourceModel())->packageAt(right);
            // The order in m_packages is based on relevancy when returned by m_backend->search()
            // Use this order to determine less than
            if (m_packages.indexOf(leftPackage) < m_packages.indexOf(rightPackage)) {
                return false;
            } else {
                return true;
            }
    } else {
        QString leftString = left.data(PackageModel::NameRole).toString();
        QString rightString = right.data(PackageModel::NameRole).toString();

        return leftString > rightString;
    }
}
