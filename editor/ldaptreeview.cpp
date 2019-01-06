#include "ldaptreeview.h"
#include "ldapeditordefines.h"

namespace ldapeditor
{
    /*
    bool CTreeFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
    {
        QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
        QVariant v = sourceModel()->data(index0, IsLeafRole);
        return v.isValid() ? !v.toBool() : false;
    }
    */
    CLdapTreeView::CLdapTreeView(QWidget *parent):QTreeView(parent)
    {

    }
    void CLdapTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
    {
        emit treeItemChanged(current,previous);
    }
}//namespace ldapeditor


