#ifndef CLDAPENTRY_H
#define CLDAPENTRY_H

#include <QObject>
#include <QString>
#include <QVector>
#include <memory>
#include "CLdapAttribute.h"
#include "CLdapException.h"

class LDAPEntry;
class LDAPConnection;

namespace ldapcore
{

class CLdapData;

/*!
 * @ingroup ldapcore
 * @brief The ldap data class
 *
 * The CLdapEntry is represented ldap entry of server.
  *
 */
class CLdapEntry : public QObject
{
	Q_OBJECT
public:

    /*!
     * \brief Constructor CLdapEntry class
     * @param parentLdapEntry pointer to CLdapEntry parent
     * @param parentLdapEntry pointer to internal instance of LDAPEntry
     * @param parent pointer to QObject parent
     */
    explicit CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject* parent = nullptr);

    /*!
     * \brief Constructor CLdapEntry class
     * @param parentLdapEntry pointer to CLdapEntry parent
     * @param rdn distinguished name to construct
     * @param parentDn distinguished name of parent entry to construct
     * @param classes list of classes (equal to 'objectClass') to construct
     * @param parent pointer to QObject parent
     */
    explicit CLdapEntry(CLdapEntry* parentLdapEntry, QString rdn, QString parentDn, QStringList classes, QObject* parent);

    /*!
     * \brief Destructor By default
     */
	~CLdapEntry();

    /*!
     * @brief Method gets parent entry ldapcore::CLdapEntry pointer
     * @return CLdapEntry* pointer to parent
     */
	CLdapEntry*             parent();

    /*!
     * @brief Method gets list of children entries
     * @return QVector<CLdapEntry*> list of CLdapEntry*
     */
	QVector<CLdapEntry*>    children();

    /*!
     * @brief Method gets list of attributes
     * @return QVector<CLdapAttribute>* pointer to list of CLdapAttribute
     */
	QVector<CLdapAttribute>* attributes();

    /*!
     * @brief Method gets list of available may attributes according schema
     * @return QVector<CLdapAttribute> list of CLdapAttribute
     */
	QVector<CLdapAttribute> availableAttributesMay();

    /*!
     * @brief Method gets list of available must attributes according schema
     * @return QVector<CLdapAttribute> list of CLdapAttribute
     */
	QVector<CLdapAttribute> availableAttributesMust();

    /*!
     * @brief Method gets must`s kind of attribute
     * @param attributeName name of attribute
     * @return bool true if attribute is must, otherwise attribute is may
     */
	bool                    isMust(std::string attributeName);

    /*!
     * @brief Method gets new`s kind of entry
     * @return bool true if entry is new, otherwise entry is already exists
     */
    bool                    isNew();

    /*!
     * @brief Method gets edit`s kind of entry
     * @return bool true if entry is editable, otherwise entry is just normal
     */
    bool                    isEdit();

    /*!
    * @brief Method sets editable kind of entry
    * @param isEdit flag to set editable kind
    */
    void                    setEditable(bool isEdit);

    /*!
     * @brief Method gets loaded state of entry
     * @return bool true if entry is loaded, otherwise entry is not loaded
     */
    bool                    isLoaded() const;

    /*!
     * @brief Method gets base distinguished name of this entry
     * @return QString distinguished name
     */
    QString                 baseDn() const;

    /*!
     * @brief Method creates new instance of CLdapAttribute by attributeName
     * @param attributeName name of attribute to creating
     * @return std::shared_ptr<CLdapAttribute> created instance of CLdapAttribute
     */
	std::shared_ptr<CLdapAttribute> createEmptyAttribute(std::string attributeName);

    /*!
     * @brief Method adds child in list of children
     * @param child pointer of CLdapEntry to adding in list of children
     */
    void                    addChild(CLdapEntry* child);

    /*!
     * @brief Method adds given attributes in list of attributes
     * @param attributes list of CLdapAttribute to adding in list of attributes
     */
    void                    addAttributes(QVector<CLdapAttribute>& attributes);

    /*!
     * @brief Method removes given child from children entries
     * @param child pointer of CLdapEntry to remove
     */
	void                    removeChild(CLdapEntry* child);

    /*!
     * @brief Method loads internal classes of this entry
     */
    void                    loadClasses();

    /*!
     * @brief Method loads internal attributes of this entry
     * @param attributes list of CLdapAttribute will be filled after this method has run
     * @param needToLoadSystemAttributes flag, if true system attributes will be filled, otherwise it will be skipped
     */
    void                    loadAttributes(QVector<CLdapAttribute>& attributes, bool needToLoadSystemAttributes = true);

    /*!
     * @brief Method sorts attributes first by must kind, second by attribute name
     */
    void                    sortAttributes();

    /*!
     * @brief Method validates given attribute according ldap schema
     * @param attr attribute to validate
     */
    void                    validateAttribute(CLdapAttribute& attr);

    /*!
     * @brief Method flush attributes cache and loads attributes from server
     */
    void                    flushAttributesCache();

    /*!
     * @brief Method loads may available attributes
     */
    void                    availableAttributesMayImpl();

    /*!
     * @brief Method loads must available attributes
     */
    void                    availableAttributesMustImpl();

    /*!
     * @brief Method gets list of ldap classes of this entry
     */
    QStringList             classes();

    /*!
     * @brief Method sets list of ldap classes of this entry
     * @param cl classes to set
     * @param updateAttributes flag, if true list of attributes will be changed based on cl, otherwise it will be skipped
     */
    void                    setClasses(QStringList cl, bool updateAttributes = false);

    /*!
     * @brief Method gets list of available ldap classes according ldap schema
     */
    QStringList             availableClasses();

    /*!
     * @brief Method gets structural class of this entry
     */
    QString                 structuralClass();

    /*!
     * @brief Method gets list of auxiliary classes of this entry
     */
    QStringList             auxiliaryClasses();

public:

    /*!
     * @brief Method gets distinguished name of this entry
     */
	QString dn();

    /*!
     * @brief Method gets relative distinguished name of this entry
     */
	QString rDn();

signals:

public slots:

private:

    /*!
     * @brief Method initializes this instance by CLdapData and base distinguished name
     * @param data pointer of CLdapData to initialize
     * @param baseDn base distinguished name to initialize
     */
    void initialize(CLdapData* data, QString baseDn);

    /*!
     * @brief Method constructs children and classes
     */
    void construct();

    /*!
     * @brief Method clear attributes and construct attributes from chache (uses if editable operation of entry is skipped)
     */

    /*!
     * @brief Method clears attributes and construct attributes from cache (uses if editable operation of entry is skipped)
     */
	void prepareAttributes();

    /*!
     * @brief Method gets internal connection pointer
     * @return LDAPConnection* pointer of LDAPConnection
     */
    LDAPConnection* connectionPtr() const;

private:

    CLdapData*            m_pData; ///< pointer of CLdapData
    CLdapEntry*           m_pParent{nullptr}; ///< pointer of CCLdapEntry, that is parent
    LDAPEntry*            m_pEntry{nullptr}; ///< pointer of CCLdapEntry, that is current
    QVector<CLdapEntry*>  m_vChildren; ///< list of children entries
    QString               m_baseDn; ///< current base distinguished name
    QString               m_rDn; ///< current relative distinguished name
    bool                  m_isNew{false}; ///< flag of new`s state
    bool                  m_isEdit{false}; ///< flag of edit`s state
    bool                  m_isLoaded{false};///< flag of load`s state

    QVector<CLdapAttribute> m_attributes; ///< list of attributes
    QVector<CLdapAttribute> m_Must; ///< list of all must attributes which can use
    QVector<CLdapAttribute> m_May; ///< list of all may attributes which can use
    QStringList        m_classes; ///< list of ldap classes

	friend class CLdapData;
	friend class CLdapAttributesModel;
	friend class CLdapTreeView;
    friend class CLdapServer;
};

}

#endif // CLDAPENTRY_H
