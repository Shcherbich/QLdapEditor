#ifndef CLDAPSCHEMA_H
#define CLDAPSCHEMA_H

#include <memory>
#include <string>
#include <tuple>
#include "CLdapAttribute.h"
#include "CLdapException.h"

class LDAPSchema;
class LDAPConnection;

namespace ldapcore
{

struct CLdapSchemaImpl;

class CLdapSchema
{
public:
    CLdapSchema();
    ~CLdapSchema();

    std::tuple<AttrType, bool> GetAttributeInfoByName(std::string attrName);

    bool isNameExist(std::string attributeName) throw (CLdapNameMissedException);
    void checkBySyntaxName(std::string attributeName, std::string value) throw (CLdapMatchRuleException);

private:
    void build(LDAPConnection* lc, std::string& baseDn);
    LDAPSchema* classesSchema();
    LDAPSchema* attributesSchema();

private:
    std::unique_ptr<CLdapSchemaImpl> m_impl;


    friend class CLdapData;
    friend class CLdapEntry;
};

}
#endif // CLDAPSCHEMA_H
