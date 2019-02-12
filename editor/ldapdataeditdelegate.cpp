/*!
\file
\brief Implementation file for attributes editor delegates

File contains  implementation for attributes editor delegates
*/

#include "ldapdataeditdelegate.h"
#include "ldapeditordefines.h"
#include "CLdapEntry.h"
#include "cmustmaymodel.h"
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
    if(!m_entry)
        return nullptr;

    editor = nullptr;
    if(index.column() == static_cast<int>(AttributeColumn::Attribute))
    {
        QComboBox* e = new QComboBox(parent);
        e->setModel(new MustMayModel(e, m_entry, m_entry->attributes()));
        e->setEditable(false);
        editor = e;
    }
    else if(index.column() == static_cast<int>(AttributeColumn::Value))
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
    else if(index.column() == static_cast<int>(AttributeColumn::Type))
    {
        QComboBox* e = new QComboBox(parent);
        int i=0;
        e->addItem("ACIItem", static_cast<int>(ldapcore::AttrType::ACIItem));
        e->addItem("AccessPoint", static_cast<int>(ldapcore::AttrType::AccessPoint));
        e->addItem("AttributeTypeDescription", static_cast<int>(ldapcore::AttrType::AttributeTypeDescription));
        e->addItem("Audio", static_cast<int>(ldapcore::AttrType::Audio));
        e->addItem("Binary", static_cast<int>(ldapcore::AttrType::Binary));
        e->addItem("BitString", static_cast<int>(ldapcore::AttrType::BitString));
        e->addItem("Boolean", static_cast<int>(ldapcore::AttrType::Boolean));
        e->addItem("Certificate", static_cast<int>(ldapcore::AttrType::Certificate));
        e->addItem("CertificateList", static_cast<int>(ldapcore::AttrType::CertificateList));
        e->addItem("CertificatePair", static_cast<int>(ldapcore::AttrType::CertificatePair));
        e->addItem("CountryString", static_cast<int>(ldapcore::AttrType::CountryString));
        e->addItem("DN", static_cast<int>(ldapcore::AttrType::DN));
        e->addItem("DataQualitySyntax", static_cast<int>(ldapcore::AttrType::DataQualitySyntax));
        e->addItem("DeliveryMethod", static_cast<int>(ldapcore::AttrType::DeliveryMethod));
        e->addItem("DirectoryString", static_cast<int>(ldapcore::AttrType::DirectoryString));
        e->addItem("DITContentRuleDescription", static_cast<int>(ldapcore::AttrType::DITContentRuleDescription));
        e->addItem("DITStructureRuleDescription", static_cast<int>(ldapcore::AttrType::DITStructureRuleDescription));
        e->addItem("DLSubmitPermission", static_cast<int>(ldapcore::AttrType::DLSubmitPermission));
        e->addItem("DSAQualitySyntax", static_cast<int>(ldapcore::AttrType::DSAQualitySyntax));
        e->addItem("DSEType", static_cast<int>(ldapcore::AttrType::DSEType));
        e->addItem("EnhancedGuide", static_cast<int>(ldapcore::AttrType::EnhancedGuide));
        e->addItem("FacsimileTelephoneNumber", static_cast<int>(ldapcore::AttrType::FacsimileTelephoneNumber));
        e->addItem("Fax", static_cast<int>(ldapcore::AttrType::Fax));
        e->addItem("GeneralizedTime", static_cast<int>(ldapcore::AttrType::GeneralizedTime));
        e->addItem("Guide", static_cast<int>(ldapcore::AttrType::Guide));
        e->addItem("IA5String", static_cast<int>(ldapcore::AttrType::IA5String));
        e->addItem("Integer", static_cast<int>(ldapcore::AttrType::Integer));
        e->addItem("Jpeg", static_cast<int>(ldapcore::AttrType::Jpeg));
        e->addItem("LDAPSyntaxDescription", static_cast<int>(ldapcore::AttrType::LDAPSyntaxDescription));
        e->addItem("LDAPSchemaDefinition", static_cast<int>(ldapcore::AttrType::LDAPSchemaDefinition));
        e->addItem("LDAPSchemaDescription", static_cast<int>(ldapcore::AttrType::LDAPSchemaDescription));
        e->addItem("MasterAndShadowAccessPoints", static_cast<int>(ldapcore::AttrType::MasterAndShadowAccessPoints));
        e->addItem("MatchingRuleDescription", static_cast<int>(ldapcore::AttrType::MatchingRuleUseDescription));
        e->addItem("MatchingRuleUseDescription", static_cast<int>(ldapcore::AttrType::MasterAndShadowAccessPoints));
        e->addItem("MailPreference", static_cast<int>(ldapcore::AttrType::MailPreference));
        e->addItem("MHSORAddress", static_cast<int>(ldapcore::AttrType::MHSORAddress));
        e->addItem("ModifyRights", static_cast<int>(ldapcore::AttrType::ModifyRights));
        e->addItem("NameAndOptionalUID", static_cast<int>(ldapcore::AttrType::NameAndOptionalUID));
        e->addItem("NameFormDescription", static_cast<int>(ldapcore::AttrType::NameFormDescription));
        e->addItem("NumericString", static_cast<int>(ldapcore::AttrType::NumericString));
        e->addItem("ObjectClassDescription", static_cast<int>(ldapcore::AttrType::ObjectClassDescription));
        e->addItem("OctetString", static_cast<int>(ldapcore::AttrType::OctetString));
        e->addItem("Oid", static_cast<int>(ldapcore::AttrType::Oid));
        e->addItem("OtherMailbox", static_cast<int>(ldapcore::AttrType::OtherMailbox));
        e->addItem("PostalAddress", static_cast<int>(ldapcore::AttrType::PostalAddress));
        e->addItem("ProtocolInformation", static_cast<int>(ldapcore::AttrType::ProtocolInformation));
        e->addItem("PresentationAddress", static_cast<int>(ldapcore::AttrType::PresentationAddress));
        e->addItem("PrintableString", static_cast<int>(ldapcore::AttrType::PrintableString));
        e->addItem("SubstringAssertion", static_cast<int>(ldapcore::AttrType::SubstringAssertion));
        e->addItem("SubtreeSpecification", static_cast<int>(ldapcore::AttrType::SubtreeSpecification));
        e->addItem("SupplierInformation", static_cast<int>(ldapcore::AttrType::SupplierInformation));
        e->addItem("SupplierOrConsumer", static_cast<int>(ldapcore::AttrType::SupplierOrConsumer));
        e->addItem("SupplierAndConsumer", static_cast<int>(ldapcore::AttrType::SupplierAndConsumer));
        e->addItem("SupportedAlgorithm", static_cast<int>(ldapcore::AttrType::SupportedAlgorithm));
        e->addItem("TelephoneNumber", static_cast<int>(ldapcore::AttrType::TelephoneNumber));
        e->addItem("TeletexTerminalIdentifier", static_cast<int>(ldapcore::AttrType::TeletexTerminalIdentifier));
        e->addItem("TelexNumber", static_cast<int>(ldapcore::AttrType::TelexNumber));
        e->addItem("UnknownText", static_cast<int>(ldapcore::AttrType::UnknownText));
        e->addItem("UtcTime", static_cast<int>(ldapcore::AttrType::UtcTime));

        e->setEditable(false);
        editor = e;
    }

    return editor;
}
void CLdapDataEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == static_cast<int>(AttributeColumn::Attribute))
    {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        for(int i=0; i< edit->count();i++)
        {
            if(edit->itemText(i) == index.data(Qt::DisplayRole).toString())
            {
                edit->setCurrentIndex(i);
                break;
            }
        }
    }
    else if(index.column() == static_cast<int>(AttributeColumn::Value))
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
    if(index.column() == static_cast<int>(AttributeColumn::Attribute))
    {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        auto text = edit->itemText(edit->currentIndex());
        model->setData(index, text, Qt::EditRole);
        auto aNew = m_entry->createEmptyAttribute(text.toStdString());
        model->setData(index.sibling(index.row(), 3), QString("%1").arg((int)aNew->type()), Qt::EditRole);

    }
    else if(index.column() == static_cast<int>(AttributeColumn::Value))
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
    else if(index.column() == static_cast<int>(AttributeColumn::Type))
    {
         QComboBox *edit = static_cast<QComboBox*>(editor);
         int i = edit->currentIndex();
         model->setData(index, edit->itemData(i), Qt::EditRole);
    }
}

void CLdapDataEditDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const
{
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

    QRegExp rxTimeZoneWest("(\\d{14}\\.\\d{3})\\d{3}\\+(\\d{2})(\\d{2})");
    QRegExp rxTimeZoneEast("(\\d{14}\\.\\d{3})\\d{3}-(\\d{2})(\\d{2})");
    QRegExp rxTimeUTC("(\\d{14}\\.\\d{3})\\d{3}Z");
    QRegExp rxTimeLocal("(\\d{12})(\\d{2})?((\\.\\d{3})\\d{3})?");
    if(rxTimeZoneWest.exactMatch(strDateTime))
    {
         QString dt = rxTimeZoneWest.cap(1);
         QString hh = rxTimeZoneWest.cap(2);
         QString mm = rxTimeZoneWest.cap(3);

         formatInfo.format = GeneralizedTimeFormat::formatTimeZoneWest;
         formatInfo.editFormat =  "yyyyMMddHHmmss.zzz";
         formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);

         int offsetUTC = hh.toInt()*60*60 + mm.toInt()*60;
         formatInfo.dateTime.setOffsetFromUtc(offsetUTC);
    }
    else if(rxTimeZoneEast.exactMatch(strDateTime))
    {
        QString dt = rxTimeZoneEast.cap(1);
        QString hh = rxTimeZoneEast.cap(2);
        QString mm = rxTimeZoneEast.cap(3);

        formatInfo.format = GeneralizedTimeFormat::formatTimeZoneEast;
        formatInfo.editFormat = "yyyyMMddHHmmss.zzz";
        formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);

        int offsetUTC = hh.toInt()*60*60 + mm.toInt()*60;
        formatInfo.dateTime.setOffsetFromUtc(-offsetUTC);
    }
    else if(rxTimeUTC.exactMatch(strDateTime))
    {
        QString dt = rxTimeUTC.cap(1);


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
        QString ms = rxTimeLocal.cap(4);

        formatInfo.format = GeneralizedTimeFormat::formatTimeZoneLocal;
        formatInfo.editFormat = "yyyyMMddHHmm";

        dt += !ss.isEmpty() ? ss : "00";
        formatInfo.editFormat = "yyyyMMddHHmmss";

        dt += !ms.isEmpty() ? ms : ".000";
        formatInfo.editFormat = "yyyyMMddHHmmss.zzz";

        formatInfo.dateTime = QDateTime::fromString(dt, formatInfo.editFormat);
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
    case GeneralizedTimeFormat::formatTimeZoneUtc:
        dt.setTimeSpec(Qt::OffsetFromUTC);
        strDateTime = dt.toString("yyyyMMddHHmmss.zzz");
        strDateTime += dt.timeZoneAbbreviation().mid(3);
        break;
    case GeneralizedTimeFormat::formatTimeZoneLocal:
        strDateTime = dt.toString("yyyyMMddHHmmss.zzz");
        break;
    default: break;
    }
    return strDateTime;
}

}//namespace ldapeditor
