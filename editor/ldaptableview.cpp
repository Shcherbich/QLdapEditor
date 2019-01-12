#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"

//#include <QtDebug>

namespace ldapeditor
{
    /*
    bool CTableFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        QModelIndex indexDN = sourceModel()->index(sourceRow, 0, sourceParent);
        QVariant vDN = sourceModel()->data(indexDN, Qt::DisplayRole);
        QModelIndex indexAttrN = sourceModel()->index(sourceRow, 1, sourceParent);
        QVariant attrN = sourceModel()->data(indexAttrN, IsLeafRole);
        bool bFilter = vDN.toString().count("=") == 1 ;
        qDebug() << vDN << bFilter;
        return bFilter;//? v.toBool() : false;
    }

    QVariant CTableFilterProxyModel::headerData(int section, Qt::Orientation orientation, int role ) const
    {
        if(orientation == Qt::Horizontal && section >=0 && section < m_Sections.size() && role == Qt::DisplayRole)
        {
            return m_Sections[section];
        }
        return QVariant();
    }
    */


    CLdapTableView::CLdapTableView(QWidget *parent) : QTableView(parent)
    , m_ldapDataDelegate(this)
    , m_defaultDelegate(this)
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setAlternatingRowColors(true);
    }

    bool CLdapTableView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)
    {
        ldapcore::AttrType type = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
        switch(type)
        {
        case ldapcore::AttrType::TelephoneNumber:
        case ldapcore::AttrType::GeneralizedTime:
            setItemDelegate(&m_ldapDataDelegate);
            break;
        default:
            setItemDelegate(&m_defaultDelegate);
        }
        return QTableView::edit(index, trigger, event);
    }
} //namespace ldapeditor
