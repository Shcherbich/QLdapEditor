#ifndef CLDAPSERVER_H
#define CLDAPSERVER_H

#include "CLdapEntry.h"
#include "CLdapAttribute.h"

class LDAPConnection;

namespace ldapcore
{

class CLdapServer
{
public:
    CLdapServer();

    void add(CLdapEntry&) noexcept(false);
    void update(CLdapEntry&) noexcept(false);
    void del(CLdapEntry&) noexcept(false);

    void addAttribute(CLdapEntry&, CLdapAttribute&) noexcept(false);
    void updateAttribute(CLdapEntry&, CLdapAttribute&) noexcept(false);
    void delAttribute(CLdapEntry&, CLdapAttribute&) noexcept(false);

};

}
#endif // CLDAPSERVER_H
