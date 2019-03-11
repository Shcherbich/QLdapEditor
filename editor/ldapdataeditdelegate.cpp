/*!
\file
\brief Implementation file for attributes editor delegates

File contains  implementation for attributes editor delegates
*/

#include "ldapdataeditdelegate.h"
#include "ldapeditordefines.h"
#include "CLdapEntry.h"
//#include "cmustmaymodel.h"
#include "attributemodelhelper.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>

#include <QRegExpValidator>
#include <QRegExp>


namespace ldapeditor
{

CLdapDataEditDelegate::CLdapDataEditDelegate(QObject *parent) : QStyledItemDelegate(parent)
{

}

void CLdapDataEditDelegate::setLdapEntry(ldapcore::CLdapEntry* entry)
{
    m_entry = entry;
}

QWidget* CLdapDataEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    if(!m_entry)
        return nullptr;

    editor = nullptr;
    // only Values column can be editted in table
    if(index.column() == static_cast<int>(AttributeColumn::Value))
    {
        m_attrType = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
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
                editor = e;
            }
            break;
            default:  break;
        }
    }

    return editor;
}
void CLdapDataEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == static_cast<int>(AttributeColumn::Value))
    {
        switch(m_attrType)
        {
        case ldapcore::AttrType::GeneralizedTime:
            {
                QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);                
                QString dt = index.model()->data(index, Qt::EditRole).toString();//.toDateTime();
                m_dateTimeFormatInfo = formatDataTimeFromString(dt);
                edit->setDisplayFormat(m_dateTimeFormatInfo.editFormat);
                edit->setDateTime(m_dateTimeFormatInfo.dateTime);
            }
            break;
        default:
            {
                QLineEdit *edit = static_cast<QLineEdit*>(editor);
                edit->setText(index.model()->data(index, Qt::EditRole).toString());
            }
        }
    }
}

void CLdapDataEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    if(index.column() == static_cast<int>(AttributeColumn::Value))
    {
        switch(m_attrType)
        {
            case ldapcore::AttrType::GeneralizedTime:
            {
                QDateTimeEdit *edit = static_cast<QDateTimeEdit*>(editor);
                m_dateTimeFormatInfo.dateTime = edit->dateTime();
                model->setData(index, makeDateTimeValue(m_dateTimeFormatInfo), Qt::EditRole);
            }
            break;
            default:
                {
                    QLineEdit *edit = static_cast<QLineEdit*>(editor);
                    model->setData(index, edit->text(), Qt::EditRole);
                }
        }
    }
}

void CLdapDataEditDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}

bool CLdapDataEditDelegate::canDeleteRow(const QModelIndex &index) const
{
    int row = index.row();
    bool b1 = index.isValid() && m_entry && m_entry->attributes() != nullptr;
    bool canDelete = b1 && m_entry->attributes()->size() > row && !(*m_entry->attributes())[row].isMust();
    return canDelete;
}

void CLdapDataEditDelegate::expandEditor() const
{
    QComboBox* qe = qobject_cast<QComboBox*>(editor);
    if (qe)
    {
        qe->showPopup();
    }    
}

CLdapDataEditDelegate::GeneralizedTimeFormatInfo CLdapDataEditDelegate::formatDataTimeFromString(const QString& strDateTime) const
{
    GeneralizedTimeFormatInfo formatInfo;

    if(strDateTime.isEmpty())
    {
        formatInfo.format = GeneralizedTimeFormat::formatTimeZoneUtc;
        formatInfo.editFormat = "yyyyMMddHHmmss.zzz";
        formatInfo.dateTime = QDateTime::currentDateTimeUtc();

        int offsetUTC = 0;
        formatInfo.dateTime.setOffsetFromUtc(offsetUTC);
    }
    else
    {
        QRegExp rxTimeZoneWest("(\\d{14}\\.\\d{3})(\\d{3})?\\+(\\d{2})(\\d{2})");
        QRegExp rxTimeZoneEast("(\\d{14}\\.\\d{3})(\\d{3})?-(\\d{2})(\\d{2})");
        QRegExp rxTimeUTC("(\\d{14})(\\.\\d{1,6})?Z");
        QRegExp rxTimeLocal("(\\d{12})(\\d{2})?(\\.\\d{1,6})?");
        if(rxTimeZoneWest.exactMatch(strDateTime))
        {
             QString dt = rxTimeZoneWest.cap(1);
             QString hh = rxTimeZoneWest.cap(3);
             QString mm = rxTimeZoneWest.cap(4);

             formatInfo.format = GeneralizedTimeFormat::formatTimeZoneWest;
             formatInfo.editFormat =  "yyyyMMddHHmmss.zzz";
             formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);

             int offsetUTC = hh.toInt()*60*60 + mm.toInt()*60;
             formatInfo.dateTime.setOffsetFromUtc(offsetUTC);
        }
        else if(rxTimeZoneEast.exactMatch(strDateTime))
        {
            QString dt = rxTimeZoneEast.cap(1);
            QString hh = rxTimeZoneEast.cap(3);
            QString mm = rxTimeZoneEast.cap(4);

            formatInfo.format = GeneralizedTimeFormat::formatTimeZoneEast;
            formatInfo.editFormat = "yyyyMMddHHmmss.zzz";
            formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);

            int offsetUTC = hh.toInt()*60*60 + mm.toInt()*60;
            formatInfo.dateTime.setOffsetFromUtc(-offsetUTC);
        }
        else if(rxTimeUTC.exactMatch(strDateTime))
        {
            QString dt = rxTimeUTC.cap(1);
            QString ff = rxTimeUTC.cap(2).left(4);
            if(ff.length() < 4)
                ff += QString(4-ff.length(), '0');
            dt += ff.isEmpty() ?".000" : ff;

            formatInfo.format = GeneralizedTimeFormat::formatTimeZoneUtc;
            formatInfo.editFormat = "yyyyMMddHHmmss.zzz";
            formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);

            int offsetUTC = 0;
            formatInfo.dateTime.setOffsetFromUtc(offsetUTC);
        }
        else if(rxTimeLocal.exactMatch(strDateTime))
        {
            QString dt = rxTimeLocal.cap(1);
            QString ss = rxTimeLocal.cap(2);
            QString ff = rxTimeLocal.cap(3);

            formatInfo.format = GeneralizedTimeFormat::formatTimeZoneLocal;
            formatInfo.editFormat = "yyyyMMddHHmm";

            dt += !ss.isEmpty() ? ss : "00";
            formatInfo.editFormat = "yyyyMMddHHmmss";

            if(ff.length() < 4)
                ff += QString(4-ff.length(), '0');
            dt += ff.isEmpty() ? ".000" : ff;
            formatInfo.editFormat = "yyyyMMddHHmmss.zzz";

            formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);
        }
    }

    return formatInfo;
}

QString CLdapDataEditDelegate::makeDateTimeValue(const GeneralizedTimeFormatInfo& formatInfo) const
{
    QDateTime dt = formatInfo.dateTime;
    QString strDateTime;
    switch(formatInfo.format)
    {
    case GeneralizedTimeFormat::formatTimeZoneWest:
    case GeneralizedTimeFormat::formatTimeZoneEast:
        dt.setTimeSpec(Qt::OffsetFromUTC);
        strDateTime = dt.toString("yyyyMMddHHmmss.zzz");
        strDateTime += dt.timeZoneAbbreviation().mid(3);
        break;
    case GeneralizedTimeFormat::formatTimeZoneUtc:
        dt.setTimeSpec(Qt::OffsetFromUTC);
        strDateTime = dt.toString("yyyyMMddHHmmss.zzz");
        strDateTime += dt.timeZoneAbbreviation().mid(3);
        strDateTime +="Z";
        break;
    case GeneralizedTimeFormat::formatTimeZoneLocal:
        strDateTime = dt.toString("yyyyMMddHHmmss.zzz");
        break;
    default: break;
    }
    return strDateTime;
}

}//namespace ldapeditor
