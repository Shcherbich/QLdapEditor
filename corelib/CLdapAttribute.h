#ifndef CLDAPATTRIBUTE_H
#define CLDAPATTRIBUTE_H

//#include <share.h>
#include <QObject>
#include <QString>

namespace ldapcore
{

enum  class AttrType
{
    UnknownText = 0,
    ACIItem = 1,
    AccessPoint = 2,
    AttributeTypeDescription = 3,
    Audio = 4,
    Binary = 5,
    BitString = 6,
    Boolean = 7,
    Certificate = 8,
    CertificateList = 9,
    CertificatePair = 10,
    CountryString = 11,
    DN = 12,
    DataQualitySyntax = 13,
    DeliveryMethod = 14,
    DirectoryString = 15,
    DITContentRuleDescription = 16,
    DITStructureRuleDescription = 17,
    DLSubmitPermission = 18,
    DSAQualitySyntax = 19,
    DSEType = 20,
    EnhancedGuide = 21,
    FacsimileTelephoneNumber = 22,
    Fax = 23,
    GeneralizedTime = 24,
    Guide = 25,
    IA5String = 26,
    Integer = 27,
    Jpeg = 28,
    LDAPSyntaxDescription = 54,
    LDAPSchemaDefinition = 56,
    LDAPSchemaDescription = 57,
    MasterAndShadowAccessPoints = 29,
    MatchingRuleDescription = 30,
    MatchingRuleUseDescription = 31,
    MailPreference = 32,
    MHSORAddress = 33,
    ModifyRights = 55,
    NameAndOptionalUID = 34,
    NameFormDescription = 35,
    NumericString = 36,
    ObjectClassDescription = 37,
    OctetString = 40,
    Oid = 38,
    OtherMailbox = 39,
    PostalAddress = 41,
    ProtocolInformation = 42,
    PresentationAddress = 43,
    PrintableString = 44,
    SubstringAssertion = 58,
    SubtreeSpecification = 45,
    SupplierInformation = 46,
    SupplierOrConsumer = 47,
    SupplierAndConsumer = 48,
    SupportedAlgorithm = 49,
    TelephoneNumber = 50,
    TeletexTerminalIdentifier = 51,
    TelexNumber = 52,
    UtcTime = 53,
};



//class LDAPEDITORCORE_SO_API CLdapAttribute : public QObject
//class CLdapAttribute : public QObject
class CLdapAttribute
{
    //Q_OBJECT
public:
    //explicit CLdapAttribute(QObject *parent = nullptr);
    //explicit CLdapAttribute(QString name, QString value, AttrType type, QObject *parent = nullptr);

    explicit CLdapAttribute();
    explicit CLdapAttribute(QString name, QString value, AttrType type, bool editable = true);

public:

    QString name() const;
    QString value() const;
    AttrType type() const;
    bool editable() const;

private:
    QString m_Name;
    QString m_Value;
    AttrType m_Type;
    bool    m_Editable;

signals:

public slots:
};

}

#endif // CLDAPATTRIBUTE_H
