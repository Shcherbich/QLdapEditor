#ifndef CLDAPTREEMODEL_H
#define CLDAPTREEMODEL_H

#include <QAbstractItemModel>
#include <QVector>

namespace ldapcore {
class CLdapEntry;
}

namespace ldapeditor
{

    class CLdapTreeModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit CLdapTreeModel(const QString& baseDN, QObject *parent = nullptr);
        bool setTopItems(QVector<ldapcore::CLdapEntry*> topItems);

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole) override;
         Qt::ItemFlags flags(const QModelIndex& index) const override;

       //  bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    private:
//        typedef struct _tLdapItem
//        {
//            QString parentDN;
//            QString itemRDN;
//            _tLdapItem* parent{nullptr};
//            QVector<_tLdapItem*> childItems;

//            _tLdapItem(const QString& _parentDN, const QString& _itemRDN):
//                parentDN(_parentDN), itemRDN(_itemRDN)
//            {

//            }

//            ~_tLdapItem()
//            {
//                for(const auto& c: childItems)
//                    delete c;
//            }

//            QString itemDN()
//            {
//                return QStringList{itemRDN, parentDN}.join(", ");
//            }

//            void addSubItem(_tLdapItem* subItem)
//            {
//                subItem->parent = this;
//                childItems.append(subItem);
//            }
//            _tLdapItem* subItem(int idx)
//            {
//                if(idx < 0 || idx >= childItems.size()) return nullptr;
//                return childItems[idx];
//            }

//            int indexOf()
//            {
//                return parent ? parent->childItems.indexOf(this) : -1;
//            }
//            int rowCount()
//            {
//                 return childItems.size() ;
//            }

//        } tLdapItem;

        //const QString& m_baseDN;
        //tLdapItem* m_rootItem{nullptr};
        //tLdapItem* m_invisibleRoot{nullptr};
        ldapcore::CLdapEntry* m_invisibleRoot{nullptr};
        QVector<ldapcore::CLdapEntry*> m_topItems;
    };
} //namespace ldapeditor


#endif // CLDAPTREEMODEL_H
