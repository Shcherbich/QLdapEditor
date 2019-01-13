#include "ldaptreeview.h"
#include "ldapeditordefines.h"

namespace ldapeditor
{
    CLdapTreeView::CLdapTreeView(QWidget *parent):QTreeView(parent)
    {

    }
    void CLdapTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
    {
        emit treeItemChanged(current,previous);
    }
}//namespace ldapeditor


