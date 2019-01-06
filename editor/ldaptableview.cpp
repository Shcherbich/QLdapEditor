#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include <QtDebug>

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
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }

} //namespace ldapeditor
