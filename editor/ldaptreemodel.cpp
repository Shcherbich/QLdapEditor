#include "ldaptreemodel.h"
#include "ldapeditordefines.h"

namespace ldapeditor
{
    CLdapTreeModel::CLdapTreeModel(const QString &baseDN, QObject *parent)
        : QAbstractItemModel(parent)
        , m_baseDN(baseDN)
    {
        m_invisibleRoot = new tLdapItem(QString(), QString());
        m_rootItem = new tLdapItem(QString(), m_baseDN);
        m_invisibleRoot->addSubItem(m_rootItem);
    }

    QModelIndex CLdapTreeModel::index(int row, int column, const QModelIndex &parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        tLdapItem* parentItem{nullptr};
        if (!parent.isValid())
            parentItem = m_invisibleRoot;
        else
            parentItem = static_cast<tLdapItem*>(parent.internalPointer());

        tLdapItem* childItem = parentItem->subItem(row);
        if (childItem)
            return createIndex(row, column, childItem);
        else
            return QModelIndex();
    }

    QModelIndex CLdapTreeModel::parent(const QModelIndex &index) const
    {
        if (!index.isValid())
           return QModelIndex();

        tLdapItem* childItem = static_cast<tLdapItem*>(index.internalPointer());
        if(!childItem)
            return QModelIndex();

        tLdapItem *parentItem = childItem->parent;
        if (parentItem == m_invisibleRoot)
            return QModelIndex();

        return createIndex(parentItem->indexOf(), 0, parentItem);
    }

    int CLdapTreeModel::rowCount(const QModelIndex &parent) const
    {
        if (!parent.isValid())
            return m_invisibleRoot->rowCount() ;

        tLdapItem* parentItem = static_cast<tLdapItem*>(parent.internalPointer());
        return parentItem ? parentItem->childItems.size() : 0 ;
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
        tLdapItem* item = static_cast<tLdapItem*>(index.internalPointer());
        if(!item) return QVariant();

        if(role == Qt::DisplayRole)
            return item->itemDN();
        if(role == ldapeditor::AttributesListRole)
            return item->itemDN().split(",");

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
             tLdapItem* item = static_cast<tLdapItem*>(index.internalPointer());
             if(item)
             {
                 item->itemRDN =value.toStringList().join(", ");
                 emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole << role);
                 return true;
             }
        }
        return false;
    }

    bool CLdapTreeModel::addLdapItem(const QString& itemDN)
    {
        tLdapItem* item = new tLdapItem(QString(),itemDN);
        m_rootItem->addSubItem(item);
        //emit dataChanged(createIndex(0,0), createIndex(0, 0));
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


