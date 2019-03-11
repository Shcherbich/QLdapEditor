#ifndef DATATYPESINTERNAL_H
#define DATATYPESINTERNAL_H

#include <memory>
#include <functional>
#include "CLdapAttribute.h"
#include "LDAPSchema.h"
#include "LDAPAttribute.h"

namespace ldapcore
{

/*!
  @brief type for comparing two std::string no case sensitivity
*/
struct compMap
{
    /*!
     * \brief Callable method for compare
     * \param lhs left side data
     * \param rhs right side data
     * \return true if lhs should be placed before rhs, false - if else
     */
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

/*!
  @brief type to storing internal data of ldap schema
*/
struct CLdapSchemaImpl
{
    /*!
     * @brief member that stored all atribute names of ldap schema
     */
	std::unique_ptr<LDAPAttribute> oc{ new LDAPAttribute() };

    /*!
     * @brief member that stored all class names of ldap schema
     */
	std::unique_ptr<LDAPAttribute> at{ new LDAPAttribute() };

    /*!
     * @brief member that stored all match rules of ldap schema
     */
	std::unique_ptr<LDAPAttribute> mr{ new LDAPAttribute() };

    /*!
     * @brief map that reflect attribute name to tuple of attribute type/user can change/syntax of attribute
     */
	std::map<std::string, std::tuple<AttrType, bool, std::string>, compMap> attr2info;

    /*!
     * @brief member that contains classes schema of openldap C++ wrapper
     */
	LDAPSchema classesSchema;

    /*!
     * @brief member that contains attributes schema of openldap C++ wrapper
     */
	LDAPSchema attributesSchema;
};


}

#endif // DATATYPESINTERNAL_H
