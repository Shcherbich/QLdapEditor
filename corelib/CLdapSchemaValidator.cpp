#include <algorithm>
#include <functional>
#include <string>
#include <regex>
#include <tuple>
#include "CLdapSchema.h"
#include "datatypesinternal.h"

namespace ldapcore
{

struct CLdapSchemaImpl;

using namespace std;
using namespace std::placeholders;

std::string CheckBitString(std::string value)
{
	for (size_t i = 0; i < value.size(); i++)
	{
		auto ch = value.at(i);
		bool b1 = ch == '0';
		bool b2 = ch == '1';
		if (!b1 && !b2)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}

std::string CheckBool(std::string value)
{
	bool b1 = value == "TRUE";
	bool b2 = value == "FALSE";

	return (b1 || b2) ? std::string("") : std::string("The value ")  + value + "' is invalid";;
}

std::string CheckCountryStringImpl(std::string value, std::string extendedSet)
{
	for (size_t i = 0; i < value.size(); i++)
	{
		auto ch = value.at(i);
		bool b1 = isalpha(ch);
		bool b2 = isdigit(ch);
		bool b3 = extendedSet.find(ch) != std::string::npos;
		if (!b1 && !b2 && !b3)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}


std::string CheckOctet(std::string value)
{
	static std::string validSet1 =  "abcdef";
	static std::string validSet2 =  "ABCDEF";

	for (size_t i = 0; i < value.size(); i++)
	{
		auto ch = value.at(i);
		bool b1 = isdigit(ch);
		bool b2 = validSet1.find(ch) != std::string::npos;
		bool b3 = validSet2.find(ch) != std::string::npos;
		if (!b1 && !b2 && !b3)
		{
			return std::string("The symbol '") + ch + "' is invalid";
		}
	}
	return std::string("");
}

static std::string validSetOfCountry  = "'()+ -.,/:?";
static std::string validSetOfCountry2 = "'()+ -.,/:?$";

std::map<std::string, std::function< std::string(std::string)> > syntaxNumber2toCheckFunction
{
	{"1.3.6.1.4.1.1466.115.121.1.6", CheckBitString},
	{"1.3.6.1.4.1.1466.115.121.1.7", CheckBool},
	{"1.3.6.1.4.1.1466.115.121.1.11", std::bind(CheckCountryStringImpl, _1, validSetOfCountry)},
	{"1.3.6.1.4.1.1466.115.121.1.14", CheckOctet},
	{"1.3.6.1.4.1.1466.115.121.1.15", CheckOctet},
	{"1.3.6.1.4.1.1466.115.121.1.22", std::bind(CheckCountryStringImpl, _1, validSetOfCountry2)},
};


void CLdapSchema::checkBySyntaxName(std::string attributeName, std::string value) throw (CLdapMatchRuleException)
{
	// for testing need to test behevior on server, so do nothing
	return;

	auto info = m_impl->attr2info.find(attributeName);
	if (info == m_impl->attr2info.end())
	{
		return;
	}
	auto f = syntaxNumber2toCheckFunction.find(std::get<2>(info->second));
	if (f != syntaxNumber2toCheckFunction.end())
	{
		auto checkReturn = f->second(value);
		if (!checkReturn.empty())
		{
			throw CLdapMatchRuleException(checkReturn.c_str());
		}
	}
}

}

