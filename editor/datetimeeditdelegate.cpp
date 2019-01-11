#include "datetimeeditdelegate.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"

#include <QDateTimeEdit>
#include <QDate>
#include <QTime>

namespace ldapeditor
{
    CDateTimeEditDelegate::CDateTimeEditDelegate(QObject *parent):QStyledItemDelegate(parent)
      , m_type(DataType::DateTime)
    {

    }

    QWidget* CDateTimeEditDelegate::createEditor(QWidget* parent,  const QStyleOptionViewItem& /* option */,  const QModelIndex&  index ) const
    {
        QDateTimeEdit *editor {nullptr};
        ldapcore::AttrType type = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
        switch(type)
        {
//        case
//        ldapcore::AttrType::Date: m_type = DataType::DateOnly;
//            editor = new QDateEdit(parent);
//            editor->setDisplayFormat("yyyy-MMM-dd");
//            break;
//        case ldapcore::AttrType::Time:
//            m_type = DataType::TimeOnly;
//            editor = new QTimeEdit(parent);
//            editor->setDisplayFormat("HH:mm:ss");
//            break;
        default:
            break;
        }

        if(editor)
        {
            editor->setFrame(false);
            if(m_type != DataType::TimeOnly)
            {
                editor->setCalendarPopup(true);
            }
        }

        return editor;
    }

    void CDateTimeEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
    {
        QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
        switch(m_type)
        {
        case DataType::DateOnly:
            edit->setDate(index.model()->data(index, Qt::EditRole).toDate());
            break;
        case DataType::TimeOnly:
            edit->setTime(index.model()->data(index, Qt::EditRole).toTime());
            break;
        }
    }

    void CDateTimeEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,  const QModelIndex &index) const
    {
        QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
        switch(m_type)
        {
        case DataType::DateOnly:
            model->setData(index, edit->date(), Qt::EditRole);
            break;
        case DataType::TimeOnly:
            model->setData(index, edit->time(), Qt::EditRole);
            break;
        }
    }

    void CDateTimeEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
    {
        editor->setGeometry(option.rect);
    }

}
