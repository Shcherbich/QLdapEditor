#ifndef CLDAPSCHEMA_H
#define CLDAPSCHEMA_H

#include <memory>
#include <string>
#include <map>
#include <tuple>
#include "CLdapAttribute.h"
#include "CLdapException.h"
#include <QVector>
#include <QString>

class LDAPSchema;
class LDAPConnection;

namespace ldapcore
{

struct CLdapSchemaImpl;


/*!
 * @ingroup ldapcore
 * @brief The ldap schema class
 *
 * The ldap schema class provides information to manipulate information of ldap schema.
 */
class CLdapSchema
{
public:
    /*!
     * \brief Constructor CLdapSchema by default
     */
	CLdapSchema();

    /*!
     * \brief Destructor CLdapSchema by default
     */
	~CLdapSchema();

    /*!
     * @brief Method gets information about attribute
     * @param attrName name of attribute to get information
     * @return std::tuple<AttrType, bool> firts is type of attribute,
     * second is true if this attribute can be changed
     * third is true if attribute can be had only single value
     */
    std::tuple<AttrType, bool, bool> attributeInfoByName(std::string attrName);

    /*!
     * @brief Method checks that given attributeName is exist on schema
     * @param attributeName name of attribute to get information
     * @return bool flag, true if name of attribute is exist on schema
     */
    bool isNameExist(std::string attributeName) noexcept(false);

    /*!
     * @brief Method validates attribute name and value by internal rules
     * @param attributeName name of attribute to validate
     * @param value value of attribute to validate
     */
    void validateAttributeByName(std::string attributeName, std::string value) noexcept(false);

    /*!
     * @brief Method gets all classes which contains on ldap schema
     * @return QStringList all classes which contains on ldap schema
     */
    QStringList classes();

    /*!
     * @brief Method gets all structural classes which contains on ldap schema
     * @return QStringList all structural classes which contains on ldap schema
     */
    QStringList structuralClasses();

    /*!
     * @brief Method gets all auxiliary classes which contains on ldap schema
     * @return QStringList all auxiliary classes which contains on ldap schema
     */
    QStringList auxiliaryClasses();

    /*!
     * @brief Method gets SUPer-or class of given class
     * @param c class to get
     * @return QString SUPer-or class
     */
    QString supByClass(QString c);

    /*!
     * @brief Method gets start preposition of given relative distinguished name
     * @param c class to get
     * @return QString preposition of relative distinguished name
     */
    QString startRdn(QString c);

    /*!
     * @brief Method gets list of auxiliary classes by SUPer-or class
     * @param sup SUPer-or class to get
     * @return QStringList list of auxiliary classes
     */
    QStringList auxiliaryClassesBySup(QString sup);

    /*!
     * @brief Method gets list of all attributes by list of classes
     * @param classes list of classes to get
     * @param attribute2value map to set beginning values
     * @return QVector<CLdapAttribute> list of CLdapAttribute
     */
    QVector<CLdapAttribute> attributeByClasses(QStringList& classes, std::map<std::string, std::string>& attribute2value);

    /*!
     * @brief Method searches all classes where given attribute can be used
     * @param attrName name of attribute to search
     * @param classesOfEntry list of classes to search
     * @return QStringList list of classes
     */
    QStringList classesByAttributeName(std::string attrName, QStringList& classesOfEntry);

    /*!
     * @brief Method gets description of class
     * @param cls name of class
     * @return QString description of class
     */
    QString classDescription(const QString& cls);

    /*!
     * @brief Method gets list of may`s CLdapAttribute by class name
     * @param cl name of class
     * @return QVector<CLdapAttribute> list of CLdapAttribute
     */
    QVector<CLdapAttribute> mayAttributesByClass(QString cl);

    /*!
     * @brief Method gets list of must`s CLdapAttribute by class name
     * @param cl name of class
     * @return QVector<CLdapAttribute> list of CLdapAttribute
     */
    QVector<CLdapAttribute> mustAttributesByClass(QString cl);

    /*!
     * @brief Method creates consistent chain of classes (like creates ldap servet of 'objectClasses' attribute) by structural and auxiliary classes
     * @param structuralCl name of structural classe
     * @param allClasses list of structural and auxiliary classes
     * @return QStringList consistent chain of classes
     */
    QStringList consistentClassesByStructuralAndOther(QString structuralCl, QStringList allClasses);

    /*!
     * @brief Method deductes structural class by list of classes
     * @param classes list of classes
     * @return QString deducted class
     */
    QString deductStructuralClass(const QStringList& classes);

private:

    /*!
     * @brief Method builds from ldap schema server to CLdapSchemaImpl class instance (need to run once in run-time)
     * @param lc internal connection
     */
    void build(LDAPConnection* lc);

    /*!
     * @brief Method gets internal class of ldap classes schema
     * @return LDAPSchema* internal class of ldap classes schema
     */
	LDAPSchema* classesSchema();

    /*!
     * @brief Method gets internal class of ldap attributes schema
     * @return LDAPSchema* internal class of ldap attributes schema
     */
	LDAPSchema* attributesSchema();

private:
    std::unique_ptr<CLdapSchemaImpl> m_impl; ///< pointer of internal class schema (pattern PImpl)


	friend class CLdapData;
	friend class CLdapEntry;
};

}
#endif // CLDAPSCHEMA_H
