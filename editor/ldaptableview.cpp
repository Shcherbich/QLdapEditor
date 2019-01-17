#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"
#include <QHeaderView>
//#include <QtDebug>

namespace ldapeditor
{
    CLdapTableView::CLdapTableView(QWidget *parent, CLdapSettings& s)
        : QTableView(parent), m_LdapSettings(s)
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

    void CLdapTableView::RestoreView()
    {
        setColumnWidth(0, m_LdapSettings.columnDn());
        setColumnWidth(1, m_LdapSettings.columnAttribute());
        setColumnWidth(2, m_LdapSettings.columnValue());
        setColumnWidth(3, m_LdapSettings.columnType());
        setColumnWidth(4, m_LdapSettings.columnSize());
        hideColumn(4);
        connect(horizontalHeader(), &QHeaderView::sectionResized, this, &CLdapTableView::OnHeaderChanged);
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

    void CLdapTableView::OnHeaderChanged(int logicalIndex, int oldSize, int newSize)
    {
        m_LdapSettings.setColumnDn(columnWidth(0));
        m_LdapSettings.setColumnAttribute(columnWidth(1));
        m_LdapSettings.setColumnValue(columnWidth(2));
        m_LdapSettings.setColumnType(columnWidth(3));
        m_LdapSettings.setColumnSize(columnWidth(4));
        m_LdapSettings.sync();
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
