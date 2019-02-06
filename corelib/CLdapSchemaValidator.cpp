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
    if (value.size() < 3)
    {
       return "The value is very small";
    }
    if (value[0] != '\'' ||
        value[value.size() - 2] != '\'' ||
        value[value.size() - 1] != 'B')
    {
        return "Incorrect syntax. Must be as '0110'B";
    }
    for (size_t i = 0; i < value.size() - 2; i++)
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
    return (b1 || b2) ? std::string("") : std::string("The value '")  + value + "' is invalid";;
}


static std::string validSetOfCountry  = "'()+ -.,/:?";
static std::string validSetOfCountry2 = "'()+ -.,/:?$";

std::string CheckCountryStringImpl(std::string value, std::string extendedSet, bool checkSizeOn2)
{
    if (checkSizeOn2 && value.size() != 2)
    {
        return "Incorrect syntax. Length must be equal 2";
    }
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

std::string ChecUTF8(std::string value)
{
    QString temp = QString::fromUtf8(value.c_str());
    return temp.length() == value.size() ? "" : "Invalid input string";
}

std::string CheckGeneralizedTime(std::string value)
{
    /*
     *Valid formats
     *
        yyyymmddhhmmss.ffffff (local time)
        yyyymmddhhmmss.ffffffZ (GMT)
        yyyymmddhhmmss.ffffff-hhmm (Time zone west)
        yyyymmddhhmmss.ffffff+hhmm (Time zone east)
        The seconds (ss) and microseconds (ffffff) can be omitted and defaults to 0.
    */

    QRegExp timeFormat("^[0-9]{14}\\.([0-9]{1,6}|[0-9]{1,6}Z|[0-9]{1,6}\\-[0-9]{4}|[0-9]{1,6}\\+[0-9]{4})$");
    timeFormat.indexIn(value.c_str());
    QStringList listName = timeFormat.capturedTexts();
    return listName.length() > 1 ? "" : "There is not a generalized time";
}

std::string CheckUTCTime(std::string value)
{
    /*
     *Valid formats
     *
        yymmddhhmmss.ffffff (local time)
        yymmddhhmmss.ffffffZ (GMT)
        yymmddhhmmss.ffffff-hhmm (Time zone west)
        yymmddhhmmss.ffffff+hhmm (Time zone east)
        The seconds (ss) and microseconds (ffffff) can be omitted and defaults to 0.
    */

    QRegExp timeFormat("^[0-9]{12}\\.([0-9]{1,6}|[0-9]{1,6}Z|[0-9]{1,6}\\-[0-9]{4}|[0-9]{1,6}\\+[0-9]{4})$");
    timeFormat.indexIn(value.c_str());
    QStringList listName = timeFormat.capturedTexts();
    return listName.length() > 1 ? "" : "There is not a generalized time";
}


#define LDAP_ASCII(c)		(!((c) & 0x80))

std::string CheckIA5String(std::string value)
{
    for (size_t i = 0; i < value.size(); i++)
    {
        auto ch = value.at(i);
        bool b1 = LDAP_ASCII(ch);
        if (!b1)
        {
            return std::string("The symbol '") + ch + "' is invalid";
        }
    }
    return std::string("");
}

std::string Check62Integer(std::string value)
{
    bool ok;
    long dec = QString(value.c_str()).toLong(&ok, 10);
    return ok ? "" : "There is not a +/- 62 digit integer";
}

std::string ChecNumericString(std::string value)
{
    QStringList list = QString(value.c_str()).split(QRegExp("\\s+"), QString::SkipEmptyParts);
    for (auto item : list)
    {
        bool ok;
        long dec = item.toLong(&ok, 10);
        if (!ok)
        {
            return QString("%1 is not a +/- 62 digit integer").arg(item).toStdString();
        }
    }
    return "";
}

struct validateInfo
{
    std::string type;
    std::function< std::string(std::string)> funcToValidate;
};

static std::map<std::string, validateInfo > syntaxNumber2toCheckFunction
{
    {"1.3.6.1.4.1.1466.115.121.1.5",  {"Binary", CheckOctet}},
    {"1.3.6.1.4.1.1466.115.121.1.6",  {"Bit String", CheckBitString}},
    {"1.3.6.1.4.1.1466.115.121.1.7",  {"Boolean", CheckBool}},
    {"1.3.6.1.4.1.1466.115.121.1.11", {"Country String", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, true)}},
    {"1.3.6.1.4.1.1466.115.121.1.14", {"Delivery Method", ChecUTF8}},
    {"1.3.6.1.4.1.1466.115.121.1.15", {"Directory String", ChecUTF8}},
    {"1.3.6.1.4.1.1466.115.121.1.22", {"Facsimile Telephone Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry2, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.24", {"Generalized Time	", CheckGeneralizedTime}},
    {"1.3.6.1.4.1.1466.115.121.1.26", {"IA5 String", CheckIA5String}},
    {"1.3.6.1.4.1.1466.115.121.1.27", {"Integer", Check62Integer}},
    {"1.3.6.1.4.1.1466.115.121.1.36", {"Numeric String", ChecNumericString}},
    {"1.3.6.1.4.1.1466.115.121.1.40", {"Octet String", CheckOctet}},
    {"1.3.6.1.4.1.1466.115.121.1.44", {"Printable String", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.50", {"Telephone Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.52", {"Telex Number", std::bind(CheckCountryStringImpl, _1, validSetOfCountry2, false)}},
    {"1.3.6.1.4.1.1466.115.121.1.53", {"UTC Time", CheckUTCTime}},
};


void CLdapSchema::checkBySyntaxName(std::string attributeName, std::string value) noexcept(false)
{
	auto info = m_impl->attr2info.find(attributeName);
	if (info == m_impl->attr2info.end())
	{
		return;
	}
	auto f = syntaxNumber2toCheckFunction.find(std::get<2>(info->second));
	if (f != syntaxNumber2toCheckFunction.end())
	{
        auto checkReturn = f->second.funcToValidate(value);
		if (!checkReturn.empty())
		{
            auto err = QString("The '%1' is not '%2' of '%3'.\nDescription: %4")
                    .arg(value.c_str()).arg(f->second.type.c_str()).arg(attributeName.c_str()).arg(checkReturn.c_str());
            throw CLdapMatchRuleException(err.toStdString().c_str());
		}
	}
}

}

