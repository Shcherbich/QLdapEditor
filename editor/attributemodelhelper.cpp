#include "attributemodelhelper.h"
#include "ldapeditordefines.h"
#include <QDateTime>
#include <QBrush>
#include <QFont>

namespace ldapeditor
{

CAttributeModelHelper::CAttributeModelHelper(ldapcore::CLdapData &ldapData):
m_LdapData(ldapData)
{
    m_attrMap[ldapcore::AttrType::UnknownText] = tAttrHelper{"UnknownText"};
    m_attrMap[ldapcore::AttrType::ACIItem] = tAttrHelper{"ACIItem"};
    m_attrMap[ldapcore::AttrType::AccessPoint] = tAttrHelper{"AccessPoint"};
    m_attrMap[ldapcore::AttrType::AttributeTypeDescription] = tAttrHelper{"AttributeTypeDescription"};
    m_attrMap[ldapcore::AttrType::Audio] = tAttrHelper{"Audio"};
    m_attrMap[ldapcore::AttrType::Binary] = tAttrHelper{"Binary"};
    m_attrMap[ldapcore::AttrType::BitString] = tAttrHelper{"BitString"};
    m_attrMap[ldapcore::AttrType::Boolean] = tAttrHelper{"Boolean"};
    m_attrMap[ldapcore::AttrType::Certificate] = tAttrHelper{"Certificate"};
    m_attrMap[ldapcore::AttrType::CertificateList] = tAttrHelper{"CertificateList"};
    m_attrMap[ldapcore::AttrType::CertificatePair] = tAttrHelper{"CertificatePair"};
    m_attrMap[ldapcore::AttrType::CountryString] = tAttrHelper{"CountryString"};
    m_attrMap[ldapcore::AttrType::DN] = tAttrHelper{"DN"};
    m_attrMap[ldapcore::AttrType::DataQualitySyntax] = tAttrHelper{"DataQualitySyntax"};
    m_attrMap[ldapcore::AttrType::DeliveryMethod] = tAttrHelper{"DeliveryMethod"};
    m_attrMap[ldapcore::AttrType::DirectoryString] = tAttrHelper{"DirectoryString"};
    m_attrMap[ldapcore::AttrType::DITContentRuleDescription] = tAttrHelper{"DITContentRuleDescription"};
    m_attrMap[ldapcore::AttrType::DITStructureRuleDescription] = tAttrHelper{"DITStructureRuleDescription"};
    m_attrMap[ldapcore::AttrType::DLSubmitPermission] = tAttrHelper{"DLSubmitPermission"};
    m_attrMap[ldapcore::AttrType::DSAQualitySyntax] = tAttrHelper{"DSAQualitySyntax"};
    m_attrMap[ldapcore::AttrType::DSEType] = tAttrHelper{"DSEType"};
    m_attrMap[ldapcore::AttrType::EnhancedGuide] = tAttrHelper{"EnhancedGuide"};
    m_attrMap[ldapcore::AttrType::FacsimileTelephoneNumber] = tAttrHelper{"FacsimileTelephoneNumber"};
    m_attrMap[ldapcore::AttrType::Fax] = tAttrHelper{"Fax"};
    m_attrMap[ldapcore::AttrType::GeneralizedTime] = tAttrHelper{"GeneralizedTime"};
    m_attrMap[ldapcore::AttrType::Guide] = tAttrHelper{"Guide"};
    m_attrMap[ldapcore::AttrType::IA5String] = tAttrHelper{"IA5String"};
    m_attrMap[ldapcore::AttrType::Integer] = tAttrHelper{"Integer"};
    m_attrMap[ldapcore::AttrType::Jpeg] = tAttrHelper{"Jpeg"};
    m_attrMap[ldapcore::AttrType::LDAPSyntaxDescription] = tAttrHelper{"LDAPSyntaxDescription"};
    m_attrMap[ldapcore::AttrType::LDAPSchemaDefinition] = tAttrHelper{"LDAPSchemaDefinition"};
    m_attrMap[ldapcore::AttrType::LDAPSchemaDescription] = tAttrHelper{"LDAPSchemaDescription"};
    m_attrMap[ldapcore::AttrType::MasterAndShadowAccessPoints] = tAttrHelper{"MasterAndShadowAccessPoints"};
    m_attrMap[ldapcore::AttrType::MatchingRuleDescription] = tAttrHelper{"MatchingRuleDescription"};
    m_attrMap[ldapcore::AttrType::MatchingRuleUseDescription] = tAttrHelper{"MatchingRuleUseDescription"};
    m_attrMap[ldapcore::AttrType::MailPreference] = tAttrHelper{"MailPreference"};
    m_attrMap[ldapcore::AttrType::MHSORAddress] = tAttrHelper{"MHSORAddress"};
    m_attrMap[ldapcore::AttrType::ModifyRights] = tAttrHelper{"ModifyRights"};
    m_attrMap[ldapcore::AttrType::NameAndOptionalUID] = tAttrHelper{"NameAndOptionalUID"};
    m_attrMap[ldapcore::AttrType::NameFormDescription] = tAttrHelper{"NameFormDescription"};
    m_attrMap[ldapcore::AttrType::NumericString] = tAttrHelper{"NumericString"};
    m_attrMap[ldapcore::AttrType::ObjectClassDescription] = tAttrHelper{"ObjectClassDescription"};
    m_attrMap[ldapcore::AttrType::OctetString] = tAttrHelper{"OctetString"};
    m_attrMap[ldapcore::AttrType::Oid] = tAttrHelper{"Oid"};
    m_attrMap[ldapcore::AttrType::OtherMailbox] = tAttrHelper{"OtherMailbox"};
    m_attrMap[ldapcore::AttrType::PostalAddress] = tAttrHelper{"PostalAddress"};
    m_attrMap[ldapcore::AttrType::ProtocolInformation] = tAttrHelper{"ProtocolInformation"};
    m_attrMap[ldapcore::AttrType::PresentationAddress] = tAttrHelper{"PresentationAddress"};
    m_attrMap[ldapcore::AttrType::PrintableString] = tAttrHelper{"PrintableString"};
    m_attrMap[ldapcore::AttrType::SubstringAssertion] = tAttrHelper{"SubstringAssertion"};
    m_attrMap[ldapcore::AttrType::SubtreeSpecification] = tAttrHelper{"SubtreeSpecification"};
    m_attrMap[ldapcore::AttrType::SupplierInformation] = tAttrHelper{"SupplierInformation"};
    m_attrMap[ldapcore::AttrType::SupplierOrConsumer] = tAttrHelper{"SupplierOrConsumer"};
    m_attrMap[ldapcore::AttrType::SupplierAndConsumer] = tAttrHelper{"SupplierAndConsumer"};
    m_attrMap[ldapcore::AttrType::SupportedAlgorithm] = tAttrHelper{"SupportedAlgorithm"};
    m_attrMap[ldapcore::AttrType::TelephoneNumber] = tAttrHelper{"TelephoneNumber"};
    m_attrMap[ldapcore::AttrType::TeletexTerminalIdentifier] = tAttrHelper{"TeletexTerminalIdentifier"};
    m_attrMap[ldapcore::AttrType::TelexNumber] = tAttrHelper{"TelexNumber"};
    m_attrMap[ldapcore::AttrType::UtcTime] = tAttrHelper{"UtcTime"};
}

void CAttributeModelHelper::setLdapEntry(ldapcore::CLdapEntry* entry)
{
    m_LdapEntry = entry;
}

QVariant CAttributeModelHelper::data(const ldapcore::CLdapAttribute&  attr, const QModelIndex &index, int role )const
{
    switch(role)
    {
    case Qt::DisplayRole:
        return displayRoleData(attr, index);
    case Qt::EditRole:
        return editRoleData(attr, index);
    case Qt::ToolTipRole:
        return tooltipRoleData(attr,index);
    case Qt::ForegroundRole:
        return foregroundRoleData(attr,index);
    case Qt::FontRole:
        return fontRoleData(attr,index);
    case ldapeditor::AttrTypeRole:
        return static_cast<int>(attr.type());
    default: break;
    }
    return QVariant();
}

bool CAttributeModelHelper::setData(ldapcore::CLdapAttribute&  attr, const QModelIndex &index, const QVariant &value, int role)
{
    switch(role)
    {
    case Qt::EditRole:
        return setEditRoleData(attr, value, index);
    default: break;
    }
    return false;
}

QVariant CAttributeModelHelper::fontRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const
{
    if (attr.isModified())
    {
        QFont font;
        font.setBold(true);
        return font;
    }
    return QVariant();
}


QString CAttributeModelHelper::attributeType2String(ldapcore::AttrType type) const
{
    return m_attrMap.value(type, tAttrHelper{"???"}).title;
}

QString CAttributeModelHelper::formatValueByType(const ldapcore::CLdapAttribute& attr) const
{
    QString retValue;
    switch(attr.type())
    {
    case ldapcore::AttrType::Binary:
        {
            QString v = attr.value();
            for(int i=0; i<v.length(); i+=2)
            {
                if(!retValue.isEmpty())
                    retValue+= " ";
                retValue+= QString("%1%2").arg(v[i]).arg(v[i+1]);
            }
        }
        break;
    case ldapcore::AttrType::GeneralizedTime:
        {
            retValue = attr.value();
            //retValue = "201902121415";
        }
        break;
    default:
        retValue = attr.value();
        break;
    }
    return retValue;
}

QString CAttributeModelHelper::displayRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const
{
    QString value = formatValueByType(attr);
    QString length = QString::number(attr.value().length());
    switch(index.column())
    {
    case static_cast<int>(AttributeColumn::Name):
        return QString("%1=%2").arg(attr.name()).arg(value);
    case static_cast<int>(AttributeColumn::Class):
        return displayClassInfo(attr);
    case static_cast<int>(AttributeColumn::Attribute):
        return attr.name();
    case static_cast<int>(AttributeColumn::Value):
        return value;
    case static_cast<int>(AttributeColumn::Type):
        return attributeType2String(attr.type());
    case static_cast<int>(AttributeColumn::Size):
        return length;
    default: break;
    }
    return QString();
}

QVariant CAttributeModelHelper::editRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const
{
    QString v = displayRoleData(attr, index);
    switch(attr.type())
    {
    case ldapcore::AttrType::Boolean:
        {
            v = v.trimmed().toLower();
            return !v.isEmpty() && v != "0" && v != "false";
        }
    case ldapcore::AttrType::Integer:
        {
            v = v.trimmed().toLower();
            return v.toInt();
        }
    case ldapcore::AttrType::GeneralizedTime:
        {
            return v.trimmed();
        }
    case ldapcore::AttrType::UtcTime:
    {
        return v.trimmed();
    }
    default:
        return v.trimmed();
    }
    return QVariant();
}

QVariant CAttributeModelHelper::tooltipRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const
{
    QString displayData = displayRoleData(attr,index);
    if (index.column() == static_cast<int>(AttributeColumn::Class))
    {
        QString str("description: ");
        QStringList l = displayData.split(";");
        for(auto s: l)
        {
            if(!str.isEmpty()) str += "\n";
            str += s == "???" ? "???" : m_LdapData.schema().classDescription(s);
        }
        return str;
    }

    if (index.column() == static_cast<int>(AttributeColumn::Attribute)) // https://github.com/Shcherbich/QLdapEditor/issues/24
    {
        QString s = attr.description();
        return QString(QObject::tr("description:\n%1")).arg(s.isEmpty() ? displayData : s);
    }

    const int chunkSize = 16*3;
    if(attr.type() == ldapcore::AttrType::Binary)
    {
        QStringList tooltipItems;
        for(int offs=0; offs<displayData.length(); offs+= chunkSize)
        {
            tooltipItems << displayData.mid(offs, chunkSize).trimmed();
        }
        return tooltipItems.join("\n");
    }
    return displayRoleData(attr,index).split(";").join("\n");
}

QVariant CAttributeModelHelper::foregroundRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const
{
    if (attr.isMust())
    {
        return QColor(Qt::blue);
    }
    switch(attr.editState())
    {
        case ldapcore::AttributeState::AttributeReadOnly:
            return QBrush(Qt::darkGray);
        case ldapcore::AttributeState::AttributeReadWrite:
            break;
            if(index.column() > static_cast<int>(AttributeColumn::Name) && index.column() != static_cast<int>(AttributeColumn::Size))
            {
                return attr.isModified() ? QBrush(Qt::blue) : QBrush(Qt::black);
            }
        case ldapcore::AttributeState::AttributeValueReadWrite:
            break;
            if(index.column() == static_cast<int>(AttributeColumn::Value))
            {
                return attr.isModified() ? QBrush(Qt::blue) : QBrush(Qt::black);
            }
        default:break;
    }
    return QBrush(Qt::black);
}

bool CAttributeModelHelper::setEditRoleData(ldapcore::CLdapAttribute &attr, const QVariant& value, const QModelIndex &index)
{
    Q_UNUSED(index);
    bool retValue {true};
    if(index.column() == static_cast<int>(AttributeColumn::Name))
    {
        attr.setName(value.toString());
        attr.setEditState(ldapcore::AttributeState::AttributeValueReadWrite);
    }
    else if(index.column() == static_cast<int>(AttributeColumn::Value))
    {
        switch(attr.type())
        {
        case ldapcore::AttrType::Boolean:
             attr.setValue(value.toBool() ? "TRUE" : "FALSE");
             break;
        case ldapcore::AttrType::Binary:
             attr.setValue(value.toString().replace(" ",""));
             break;
        case ldapcore::AttrType::Integer:
             attr.setValue(QString::number(value.toInt()));
             break;
        case ldapcore::AttrType::GeneralizedTime:
             attr.setValue(value.toString());
             //attr.setValue(value.toDateTime().toString(LDAP_EDITOR_SERVER_DATETIME_FORMAT) + LDAP_EDITOR_SERVER_POSTFIX);
            break;
        case ldapcore::AttrType::UtcTime:
             attr.setValue(value.toString());
             //attr.setValue(value.toDateTime().toUTC().toString(LDAP_EDITOR_SERVER_DATETIME_FORMAT) + LDAP_EDITOR_SERVER_POSTFIX);
            break;
        default:
            attr.setValue(value.toString());
        }
    }
     else if(index.column() == static_cast<int>(AttributeColumn::Type))
    {
        attr.setType(static_cast<ldapcore::AttrType>(value.toInt()));
    }

    return retValue;
}

QString CAttributeModelHelper::displayClassInfo(const ldapcore::CLdapAttribute &attr) const
{
    QString str;
    if(m_LdapEntry)
    {
        for(auto s : attr.classes())
             str += !str.isEmpty() ? (";" + s) : s;
    }
    return str.isEmpty() ? "???" : str;
}

} //namespace ldapeditor
