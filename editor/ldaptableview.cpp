#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"

//#include <QtDebug>

namespace ldapeditor
{
    CLdapTableView::CLdapTableView(QWidget *parent) : QTableView(parent)
    , m_ldapDataDelegate(this)
    , m_defaultDelegate(this)
    , m_contextMenu(this)
    , m_newAttr(new QAction("New attribute", this))
    , m_delAttr(new QAction("Delete attribute", this))
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setAlternatingRowColors(true);
        setSortingEnabled(false);

        connect(m_newAttr, &QAction::triggered, this, &CLdapTableView::onNewAttribute);
        connect(m_delAttr, &QAction::triggered, this, &CLdapTableView::onDeleteAttribute);

        m_contextMenu.addAction(m_newAttr);
        m_contextMenu.addAction(m_delAttr);
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QTableView::customContextMenuRequested, this, &CLdapTableView::customContextMenuRequested);
    }

    void CLdapTableView::setLdapEntry(ldapcore::CLdapEntry* entry)
    {
        m_ldapDataDelegate.setLdapEntry(entry);
    }

    bool CLdapTableView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)
    {
        if(index.column() == 1)
        {
            setItemDelegate(&m_ldapDataDelegate);
        }
        else if(index.column() == 2)
        {
            ldapcore::AttrType type = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
            switch(type)
            {
            case ldapcore::AttrType::Binary:
            case ldapcore::AttrType::TelephoneNumber:
            case ldapcore::AttrType::GeneralizedTime:
                setItemDelegate(&m_ldapDataDelegate);
                break;
            default:
                setItemDelegate(&m_defaultDelegate);
            }
        }
        else if(index.column() == 3)
        {
            setItemDelegate(&m_ldapDataDelegate);
        }
        return QTableView::edit(index, trigger, event);
    }

    void CLdapTableView::customContextMenuRequested(QPoint pos)
    {
        QModelIndex index = indexAt(pos);
        m_delAttr->setData(index);
        m_delAttr->setEnabled(index.isValid());
        m_contextMenu.popup(viewport()->mapToGlobal(pos));
    }

    void CLdapTableView::onNewAttribute()
    {
        model()->insertRows(model()->rowCount(), 1);
    }

    void CLdapTableView::onDeleteAttribute()
    {
        QAction* a = qobject_cast<QAction*>(sender());
        QModelIndex i = a->data().toModelIndex();
        if(i.isValid())
        {
            model()->removeRows(i.row(), 1, i.parent());
        }
    }
} //namespace ldapeditor
