#ifndef CLDAPSERVER_H
#define CLDAPSERVER_H

#include "CLdapEntry.h"
#include "CLdapAttribute.h"

class LDAPConnection;

namespace ldapcore
{

/*!
 * @ingroup ldapcore
 * @brief The ldap data class
 *
 * The ldap server is main backend class for executeting and processing result of server-base procedures.
 * One instance of CLdapServer is real-time enough.
 *
 */
class CLdapServer
{
public:
    /*!
     * \brief Constructor CLdapServer by default
     */
    CLdapServer();

    /*!
     * @brief Method To add new entry with filled attributes to ldap server
     * @param entry The entry to added
     */
    void add(CLdapEntry& entry) noexcept(false);

    /*!
     * @brief Method To update entry with filled attributes on ldap server
     * @param entry The entry to update
     */
    void update(CLdapEntry& entry) noexcept(false);

    /*!
     * @brief Method To delete entry from ldap server
     * @param entry The entry to delete
     */
    void del(CLdapEntry&) noexcept(false);

    /*!
     * @brief Method To add attribute of entry to ldap server
     * @param entry The entry that contains given attribute
     * @param attribute The attribute to add
     */
    void addAttribute(CLdapEntry& entry, CLdapAttribute& attribute) noexcept(false);

    /*!
     * @brief Method To update attribute of entry on ldap server
     * @param entry The entry that contains given attribute
     * @param name The name of attribute to update
     * @param values The values of attributes to update
     */
    void updateAttributes(CLdapEntry& entry, QString name, const QVector<ldapcore::CLdapAttribute>& values) noexcept(false);

    /*!
     * @brief Method To delete attribute of entry from ldap server
     * @param entry The entry that contains given attribute
     * @param attribute The attribute to delete
     */
    void delAttribute(CLdapEntry& entry, CLdapAttribute& attribute) noexcept(false);

    /*!
     * @brief entryOfUser The entry that contains user to enable
     */
    void enableUser(CLdapEntry& entryOfUser) noexcept(false);

    /*!
     * @brief entryOfUser The entry that contains user to disable
     */
    void disableUser(CLdapEntry& entryOfUser) noexcept(false);
};

}
#endif // CLDAPSERVER_H
