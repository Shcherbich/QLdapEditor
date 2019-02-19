/*!
\file
\brief Implementation file for LDAP tree models

File contains  implementation for LDAP tree models
*/

#include "ldaptreemodel.h"
#include "ldapeditordefines.h"
#include "CLdapEntry.h"

#include <QIcon>

namespace ldapeditor
{
    bool CLdapTreeProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
    {
        QString dnLeft = sourceModel()->data(source_left,Qt::DisplayRole).toString();
        QString dnRight = sourceModel()->data(source_right,Qt::DisplayRole).toString();
        return dnLeft.compare(dnRight, Qt::CaseInsensitive) < 0;
    }

    CLdapTreeModel::CLdapTreeModel(const QString &baseDN, QObject *parent)
        : QAbstractItemModel(parent)
        //, m_baseDN(baseDN)
    {
       // m_invisibleRoot = new tLdapItem(QString(), QString());
       // m_rootItem = new tLdapItem(QString(), m_baseDN);
      //  m_invisibleRoot->addSubItem(m_rootItem);
    }

    QModelIndex CLdapTreeModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        if (!parent.isValid())
        {
            return createIndex(row, column, m_topItems[row]);
        }
        else
        {
            ldapcore::CLdapEntry* parentItem = static_cast<ldapcore::CLdapEntry*>(parent.internalPointer());
            ldapcore::CLdapEntry* childItem = parentItem->children().at(row);
            if (childItem)
                return createIndex(row, column, childItem);
            else
                return QModelIndex();
        }
    }

    QModelIndex CLdapTreeModel::parent(const QModelIndex &index) const
    {
        if (!index.isValid())
           return QModelIndex();

        ldapcore::CLdapEntry* childItem = static_cast<ldapcore::CLdapEntry*>(index.internalPointer());
        if(!childItem)
            return QModelIndex();

        ldapcore::CLdapEntry *parentItem = childItem->parent();
        if (parentItem == m_invisibleRoot)
            return QModelIndex();

        if(!parentItem->parent())
        {
            int idx = m_topItems.indexOf(parentItem);
            return createIndex(idx, 0, parentItem);
        }
        QVector<ldapcore::CLdapEntry*> items = parentItem->parent()->children();
        int idx = items.indexOf(parentItem);
        return createIndex(idx, 0, parentItem);
    }

    int CLdapTreeModel::rowCount(const QModelIndex &parent) const
    {
        if (!parent.isValid())
            return m_topItems.size();

        ldapcore::CLdapEntry* parentItem = static_cast<ldapcore::CLdapEntry*>(parent.internalPointer());
        if(parentItem)
            return parentItem->isLoaded() ? parentItem->children().size() : 0 ;
        return 0;
        //return parentItem ? parentItem->children().size() : 0 ;
    }

    int CLdapTreeModel::columnCount(const QModelIndex &parent) const
    {
        return 1;
    }

    QVariant CLdapTreeModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        // FIXME: Implement me!
        if (index.column() != 0) return QVariant();
        ldapcore::CLdapEntry* item = static_cast<ldapcore::CLdapEntry*>(index.internalPointer());
        if(!item) return QVariant();

        if(role == LdapTreeRoles::TreeDnRole)
        {
            return item->dn();
        }
        else if(role == Qt::DisplayRole)
        {
            return item->rDn();
        }
        else if(role == Qt::DecorationRole)
        {
            if(!item->parent()) return QIcon(":/home");

            QStringList classes;
            for(auto& c: item->classes())
                classes.append(c);

            // icon is defined by top + one of other classes
            if(classes.contains("top",Qt::CaseInsensitive))
            {
                if(classes.contains("group",Qt::CaseInsensitive))
                    return QIcon(":/group");
                else if(classes.contains("person",Qt::CaseInsensitive))
                    return QIcon(":/person");
                else if(classes.contains("organizationalUnit",Qt::CaseInsensitive))
                    return QIcon(":/diagram");
                else
                    return QIcon(":/folder");
            }

        }

        return QVariant();
    }

    Qt::ItemFlags CLdapTreeModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags defaultFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable};
        return defaultFlags;
    }

    bool CLdapTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid())
            return false;

        if(role == ldapeditor::AttributesListRole)
        {
             ldapcore::CLdapEntry* item = static_cast<ldapcore::CLdapEntry*>(index.internalPointer());
             if(item)
             {
                 emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole << role);
                 return true;
             }
        }
        return false;
    }

    bool CLdapTreeModel::setTopItems(QVector<ldapcore::CLdapEntry*> topItems)
    {
        beginResetModel();
        m_topItems = topItems;
        endResetModel();
        return true;
    }

    void CLdapTreeModel::removeAttribute(QString name)
    {
        emit onRemovingAttribute(name);
    }

    void CLdapTreeModel::addAttribute(QString name)
    {
        emit onAddAttribute(name);
    }

    QModelIndex CLdapTreeModel::addNewEntry(QModelIndex parent, QString rdn, QString dn, QVector<QString>& classes, QVector<ldapcore::CLdapAttribute>& attributes)
    {
        ldapcore::CLdapEntry* parentEntry = static_cast<ldapcore::CLdapEntry*>(parent.internalPointer());
        if (!parentEntry)
        {
            return QModelIndex();
        }

        int  row = rowCount(parent);
        int count = 1;

        ldapcore::CLdapEntry* addEntry = new ldapcore::CLdapEntry(parentEntry, rdn, dn, classes, nullptr);
        addEntry->addAttributes(attributes);

        beginInsertRows(parent, row, row + count - 1);
        parentEntry->addChild(addEntry);
        endInsertRows();

        QModelIndex idxAdd = index(row, 0, parent);
        emit dataChanged(idxAdd, idxAdd, QVector<int>() << Qt::DisplayRole << Qt::DecorationRole << LdapTreeRoles::TreeDnRole);
        return idxAdd;
    }

    bool CLdapTreeModel::removeRows(int row, int count, const QModelIndex &parent)
    {
        ldapcore::CLdapEntry* parentEntry = static_cast<ldapcore::CLdapEntry*>(parent.internalPointer());
        if(parentEntry)
        {
            QModelIndex childIdx = index(row, 0, parent);
            if(childIdx.isValid())
            {
                ldapcore::CLdapEntry* childEntry = static_cast<ldapcore::CLdapEntry*>(childIdx.internalPointer());
                if(childEntry)
                {
                    beginRemoveRows(parent, row, row + count -1);
                    parentEntry->removeChild(childEntry);
                    endRemoveRows();

                    emit dataChanged(parent, parent);
                    return true;
                }
            }
        }
        return false;
    }

    bool CLdapTreeModel::canFetchMore(const QModelIndex &parent) const
    {
        return true;
    }

    void CLdapTreeModel::fetchMore(const QModelIndex& parent)
    {
    }

    bool CLdapTreeModel::hasChildren(const QModelIndex &parent) const
    {
        bool bRet {true};
        ldapcore::CLdapEntry* parentEntry = static_cast<ldapcore::CLdapEntry*>(parent.internalPointer());
        if(parentEntry)
        {
            if(parentEntry->isLoaded() )
            {
                bRet = !parentEntry->children().isEmpty();
            }
        }
        return bRet;
    }

    bool CLdapTreeModel::insertRows(int row, int count, const QModelIndex& parent)
    {
        beginInsertRows(parent, row, row + count - 1);
        endInsertRows();

        QModelIndex idxFrom = index(row, 0, parent);
        QModelIndex idxTo = index(row + count - 1, 0, parent);
        emit dataChanged(idxFrom, idxTo, QVector<int>() << Qt::DisplayRole << Qt::DecorationRole);
        return true;
    }
} //namespace ldapeditor


