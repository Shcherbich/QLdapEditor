#include "ldapdataeditdelegate.h"
#include "ldapeditordefines.h"
#include "CLdapEntry.h"

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
    QWidget* editor{nullptr};
    if(index.column() == 1)
    {
        QComboBox* e = new QComboBox(parent);
        int i=0;
        QVector<ldapcore::CLdapAttribute> attrs = m_entry->availableAttributes();
        for(int i=0; i< attrs.size();i++)
        {
            e->addItem(attrs[i].name());
        }
        e->setEditable(false);
        editor = e;
    }
    else if(index.column() == 2)
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
                e->setDisplayFormat("yyyy.MM.dd HH:mm:ss.zzz");
                editor = e;
            }
            break;
            default:  break;
        }
    }
    else if(index.column() == 3)
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
    if(index.column() == 1)
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
    else if(index.column() == 2)
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
    else if(index.column() == 3)
    {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        for(int i=0; i< edit->count();i++)
        {
            if(edit->itemData(i).toInt() == index.data(AttrTypeRole).toInt())
            {
                edit->setCurrentIndex(i);
                break;
            }
        }
    }


}

void CLdapDataEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    if(index.column() == 1)
    {
        QComboBox *edit = static_cast<QComboBox*>(editor);
        int i = edit->currentIndex();
        model->setData(index, edit->itemText(i), Qt::EditRole);
    }
    else if(index.column() == 2)
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
    else if(index.column() == 3)
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
}//namespace ldapeditor
