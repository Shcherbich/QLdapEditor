#ifndef CLDAPATTRIBUTE_H
#define CLDAPATTRIBUTE_H

#include <QObject>
#include <QVector>
#include <QString>

namespace ldapcore
{
/*!
 * @ingroup ldapcore
 * @brief The AttrType enumeration
 *
 * Presents all openldap attribute types.
 * according rfc2252
 */
enum class AttrType
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

/*!
 * @ingroup ldapcore
 * @brief The AttributeState enumeration
 *
 * Presents all states of attribute.
 */
enum  class AttributeState
{
	AttributeReadOnly = 0,
	AttributeReadWrite,
	AttributeValueReadWrite
};

/*!
 * @ingroup ldapcore
 * @brief The ldap attribute class
 *
 * The ldap attribute class provides information about attribute e.g. name, value, type etc.
 */

class CLdapAttribute
{
public:

    /*!
     * \brief Constructor CLdapAttribute by default
     */
	explicit CLdapAttribute();

    /*!
     * \brief Constructor CLdapAttribute
     * \param name name of attribute
     * \param value value of attribute
     * \param type type of attribute
     * \param isMust true if is must-attribute, false if is may-attribute
     * \param desc description of attribute
     * \param classes list of classes which can contain this attribute
     * \param isSingle true if is must-line value is supported, otherwise false
     * \param editState current editable state of attribute
     */
    explicit CLdapAttribute(QString name, QString value, AttrType type, bool isMust, QString desc,
                            QStringList& classes, bool isSingle,
                            AttributeState editState = AttributeState::AttributeValueReadWrite);

    /*!
     * \brief Constructor Copy constructor of CLdapAttribute
     * \param src Source for coping
     */
    CLdapAttribute(const CLdapAttribute& src);

    /*!
     * \brief Constructor Move constructor of CLdapAttribute
     * \param temp Source for coping
     */
    CLdapAttribute(CLdapAttribute&& temp);

    /*!
     * \brief Operator Assignment operator
     * \param o Source for assignment
     */
    CLdapAttribute& operator = (const CLdapAttribute& o);

    /*!
     * \brief Operator Move assignment operator
     * \param o Temporary object for assignment
     */
    CLdapAttribute& operator = (CLdapAttribute&& o);

public:

    /*!
     * @brief Method returns QString representation of attribute name
     * @return QString name
     */
	QString name() const;

    /*!
     * @brief Method returns QString representation of attribute value
     * @return QString value
     */
	QString value() const;

    /*!
     * @brief Method returns AttrType representation of attribute type
     * @return AttrType type
     */
	AttrType type() const;

    /*!
     * @brief Method returns AttributeState representation of attribute editState
     * @return AttributeState editState
     */
	AttributeState editState() const;

    /*!
     * @brief Method returns QString representation of attribute description
     * @return QString description
     */
    QString description() const;

    /*!
     * @brief Method returns bool representation of attribute modified state
     * @return bool isModified
     */
	bool isModified() const;

    /*!
     * @brief Method returns true if is must-attribute, false if is may-attribute
     * @return bool isMust
     */
    bool isMust() const;

    /*!
     * @brief Method returns true if is must-attribute, false if is may-attribute
     * @return bool isSingle
     */
    bool isSingle() const;

    /*!
     * @brief Method returns list of classes which can contain this attribute
     * @return QStringList classes
     */
    QStringList classes() const;

    /*!
     * @brief Method sets name of attribute
     * @param name name to set
     */
	void setName(const QString& name);

    /*!
     * @brief Method sets value of attribute
     * @param value value to set
     */
	void setValue(const QString& value);

    /*!
     * @brief Method sets type of attribute
     * @param type type to set
     */
	void setType(AttrType type);

    /*!
     * @brief Method sets editable state of attribute
     * @param state state to set
     */
    void setEditState(AttributeState state);

    /*!
     * @brief Method sets description of attribute
     * @param desc desc to set
     */
    void setDescription(QString& desc);


    /*!
     * @brief Method sets list of classes which can contain this attribute
     * @param list classes to set
     */
    void setClasses(QStringList& list);

    /*!
     * @brief Method returns boolean flag is atrribute explicitly marked as ignored
     * @return QStringList classes
     */
    inline bool isIgnore() const {return m_isIgnore;}

    /*!
     * @brief Method sets is attribute marked as ignored
     * @param ignore ignore or not attribute
     */
    inline void setIsIgnore(bool ignore) {m_isIgnore = ignore;}

private:
    /*!
     * @brief member Name object
     */
    QString     m_Name;

    /*!
     * @brief member Value string
     */
    QString     m_Value;

    /*!
     * @brief member Type enumeration
     */
    AttrType    m_Type{AttrType::UnknownText};

    /*!
     * @brief member Must bool
     */
    bool        m_isMust{false};

    /*!
     * @brief member Single bool
     */
    bool        m_isSingle{true};

    /*!
     * @brief member Description string
     */
    QString     m_Description;

    /*!
     * @brief member list of classes which can contain this attribute
     */
    QStringList m_Classes;

    /*!
     * @brief member Editable state enumeration
     */
	AttributeState    m_editState;

    /*!
     * @brief member Modified bool
     */
    bool        m_isModified{false};


    /*!
     * @brief member is attribute explicitly marked as ignored
     */
    bool        m_isIgnore{false};

private:

    /*!
     * @brief Method To validate new value to set
     * @param value New value to set
     * @return bool true if validation is rights, false if validation is bad
     */
    bool validateValue(const QString& value);

signals:

public slots:

    friend class CLdapEntry;

};

}

#endif // CLDAPATTRIBUTE_H
