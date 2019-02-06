#ifndef DATATYPESINTERNAL_H
#define DATATYPESINTERNAL_H

#include <memory>
#include <functional>
#include "CLdapAttribute.h"
#include "LDAPSchema.h"
#include "LDAPAttribute.h"

namespace ldapcore
{

struct compMap
{
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

struct CLdapSchemaImpl
{
	std::unique_ptr<LDAPAttribute> oc{ new LDAPAttribute() };
	std::unique_ptr<LDAPAttribute> at{ new LDAPAttribute() };
	std::unique_ptr<LDAPAttribute> mr{ new LDAPAttribute() };

	std::map<std::string, std::tuple<AttrType, bool, std::string>, compMap> attr2info;

	LDAPSchema classesSchema;
	LDAPSchema attributesSchema;
};


}

#endif // DATATYPESINTERNAL_H
