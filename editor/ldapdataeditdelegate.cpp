#include "ldapdataeditdelegate.h"
#include "ldapeditordefines.h"


#include <QLineEdit>
#include <QDateTimeEdit>

#include <QRegExpValidator>
#include <QRegExp>

namespace ldapeditor
{

CLdapDataEditDelegate::CLdapDataEditDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

QWidget* CLdapDataEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    m_attrType = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
    QWidget* editor{nullptr};
    switch(m_attrType)
    {
        case ldapcore::AttrType::TelephoneNumber:
        {
            QLineEdit* e  = new QLineEdit(parent);
            e->setValidator(new QRegExpValidator(QRegExp("[\\d'\\(\\)+-\\.,/:\\? ]+")));
            editor = e;
        }
        break;
        case ldapcore::AttrType::Binary:
        {
            QLineEdit* e  = new QLineEdit(parent);
            e->setValidator(new QRegExpValidator(QRegExp("([0-9A-F]{2} ?)+")));
            editor = e;
        }
        break;
        case ldapcore::AttrType::GeneralizedTime:
        {
            QDateTimeEdit* e  = new QDateTimeEdit(parent);
            e->setDisplayFormat("yyyy.MM.dd HH:mm:ss.zzz");
            editor = e;
        }
        break;
        default:  break;
    }
    return editor;
}
void CLdapDataEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch(m_attrType)
    {
    case ldapcore::AttrType::GeneralizedTime:
        {
            QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
            edit->setDateTime(index.model()->data(index, Qt::EditRole).toDateTime());
        }
        break;
    default:
        {
            QLineEdit *edit = static_cast<QLineEdit*>(editor);
            edit->setText(index.model()->data(index, Qt::EditRole).toString());
        }
    }

}

void CLdapDataEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    switch(m_attrType)
    {
        case ldapcore::AttrType::GeneralizedTime:
        {
            QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
            model->setData(index, edit->dateTime(), Qt::EditRole);
        }
        break;
        default:
            {
                QLineEdit *edit = static_cast<QLineEdit*>(editor);
                model->setData(index, edit->text(), Qt::EditRole);
            }
    }
}

void CLdapDataEditDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}
}//namespace ldapeditor
