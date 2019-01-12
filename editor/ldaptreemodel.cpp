#include "ldaptreemodel.h"
#include "ldapeditordefines.h"
#include "CLdapEntry.h"

#include <QIcon>

namespace ldapeditor
{
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
        return parentItem ? parentItem->children().size() : 0 ;
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

        if(role == Qt::DisplayRole)
        {
            return item->rDn();
        }
        if(role == Qt::DecorationRole)
        {
            if(!item->parent()) return QIcon(":/home");
            for(auto& a: *item->attributes())
            {
                if(a.value().contains("groupOfUniqueNames;"))
                    return QIcon(":/group");
                if(a.value().contains("inetOrgPerson;organizationalPerson;person;"))
                           return QIcon(":/person");
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
                 //item->itemRDN =value.toStringList().join(", ");
                 emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole << role);
                 return true;
             }
        }
        return false;
    }

    bool CLdapTreeModel::setTopItems(QVector<ldapcore::CLdapEntry*> topItems)
    {
        m_topItems = topItems;
        //ldapcore::CLdapEntry* item = new tLdapItem(QString(),entry->getDn());
        //m_rootItem->addSubItem(item);
        emit dataChanged(createIndex(0,0), createIndex(m_topItems.size(), 0));
        return true;
    }

    //bool CLdapTreeModel::insertRows(int row, int count, const QModelIndex &parent)
    // {
    //      tLdapItem* parentItem = static_cast<tLdapItem*>(parent.internalPointer());
    //      beginInsertRows(parent, row, row + count - 1);
    //          parentItem->addSubItem(itemType, itemTitle);
    //      endInsertRows();
    // }

} //namespace ldapeditor


