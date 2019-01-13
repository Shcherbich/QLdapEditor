#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"

//#include <QtDebug>

namespace ldapeditor
{
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
        case ldapcore::AttrType::Binary:
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
