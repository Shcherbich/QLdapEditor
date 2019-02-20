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
    explicit CLdapEntry(CLdapEntry* parentLdapEntry, QString rdn, QString parentDn, QVector<QString>& classes, QObject* parent);

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
	bool                    isMust(std::string attributeName);
    bool                    isNew();
    bool                    isEdit();
    void                    setEditable(bool isEdit);
    bool                    isLoaded() const;
	std::shared_ptr<CLdapAttribute> createEmptyAttribute(std::string attributeName);

    void                    addChild(CLdapEntry* child);
    void                    addAttributes(QVector<CLdapAttribute>&);
	void                    removeChild(CLdapEntry* child);
    void                    loadClasses();
    void                    loadAttributes(QVector<CLdapAttribute>&, bool needToLoadSystemAttributes = true);
    void                    sortAttributes();
    void                    validateAttribute(CLdapAttribute& attr);
    void                    flushAttributeCache();

    void                    availableAttributesMayImpl();
    void                    availableAttributesMustImpl();

    QVector<QString>        classes();
    void                    setClasses(QVector<QString>&);
    QVector<QString>        availableClasses();
    QString                 structuralClass();
    QVector<QString>        auxiliaryClasses();

public:
	QString dn();
	QString rDn();

signals:

public slots:

private:
    void initialize(CLdapData* data, QString baseDn);
    void construct();
	void prepareAttributes();
    LDAPConnection* connectionPtr() const;

private:
    CLdapData*            m_pData;
	CLdapEntry*           m_pParent{nullptr};
	LDAPEntry*            m_pEntry{nullptr};
    QVector<CLdapEntry*>  m_vChildren;
	QString               m_baseDn;
	QString               m_rDn;
	bool                  m_isNew{false};
    bool                  m_isEdit{false};
    bool                  m_isLoaded{false};

	QVector<CLdapAttribute> m_attributes;
	QVector<CLdapAttribute> m_Must;
	QVector<CLdapAttribute> m_May;
	QVector<QString>        m_classes;

	friend class CLdapData;
	friend class CLdapAttributesModel;
	friend class CLdapTreeView;
    friend class CLdapServer;
};

}

#endif // CLDAPENTRY_H
