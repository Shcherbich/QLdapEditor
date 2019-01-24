#include "ldaptreeview.h"
#include "ldapeditordefines.h"
#include "ldapnewentrydialog.h"
#include "CLdapEntry.h"

namespace ldapeditor
{
    CLdapTreeView::CLdapTreeView(QWidget *parent)
        :QTreeView(parent)
        , m_newEntry(new QAction("New entry", this))
    {
        m_contextMenu.addAction(m_newEntry);
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QTreeView::customContextMenuRequested, this, &CLdapTreeView::customContextMenuRequested);
        connect(m_newEntry, &QAction::triggered, this, &CLdapTreeView::onNewEntry);

    }
    void CLdapTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
    {
        emit treeItemChanged(current,previous);
    }

    void CLdapTreeView::onNewEntry()
    {
        auto index = currentIndex();
        if (!index.isValid())
        {
            return;
        }
        //QVariant data = model()->data(index);
        ldapcore::CLdapEntry* currentEntry = static_cast<ldapcore::CLdapEntry*>(index.internalPointer());
        if(!currentEntry)
        {
            return;
        }

        auto dn = currentEntry->dn();
        ldapeditor::CLdapNewEntryDialog dialog(nullptr, dn, currentEntry->availableClasses());
        if (dialog.exec() == QDialog::Rejected)
        {
           return;
        }

        auto selected = dialog.selectedClasses();
        auto rdn = dialog.rdn();


    }

    void CLdapTreeView::customContextMenuRequested(QPoint pos)
    {
        QModelIndex index = indexAt(pos);
        m_contextMenu.popup(viewport()->mapToGlobal(pos));
    }

}//namespace ldapeditor


