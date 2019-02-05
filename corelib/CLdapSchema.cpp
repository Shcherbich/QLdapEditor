#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <regex>
#include <tuple>
#include <QRegExp>
#include <QDebug>
#include "CLdapSchema.h"
#include "LDAPAttribute.h"
#include "LDAPConnection.h"
#include "LDAPEntry.h"
#include "LDAPSchema.h"
#include <string.h>
#include <string>

struct comp
{
	bool operator()(const std::string& lhs, const std::string& rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

using uEntry = std::shared_ptr<LDAPEntry>;

namespace ldapcore
{

std::tuple<AttrType, std::string> FromAttributeString(std::string attr)
{
    static QRegExp re("SYNTAX\\s+'*([0-9a-zA-Z\\.]+)'*");
    re.indexIn(attr.c_str());
    QStringList listName = re.capturedTexts();
    auto syntax = listName[1].toStdString();
    if (syntax[2] == '2') // Microsoft specifed
    {
        static std::map<std::string, AttrType> microsoftMap
        {
            {"1.2.840.113556.1.4.903", AttrType::OctetString},
            {"1.2.840.113556.1.4.904", AttrType::IA5String},
            {"1.2.840.113556.1.4.905", AttrType::PrintableString},
            {"1.2.840.113556.1.4.906", AttrType::Integer},
            {"1.2.840.113556.1.4.907", AttrType::OctetString},
        };
        auto f = microsoftMap.find(syntax);
        if (f != microsoftMap.end())
        {
            return std::tuple<AttrType, std::string>{f->second, syntax};
        }
        return std::tuple<AttrType, std::string>{AttrType::UnknownText, ""};
    }

    // string representation
    if (!isdigit(syntax[0]))
    {
        static std::map<std::string, AttrType> namedMap
        {
            {"OctetString", AttrType::OctetString}
        };
        auto f = namedMap.find(syntax);
        if (f != namedMap.end())
        {
            return std::tuple<AttrType, std::string>{f->second, syntax};
        }
        return std::tuple<AttrType, std::string>{AttrType::UnknownText, ""};
    }

    std::size_t dot = syntax.find_last_of(".");
    auto s = syntax.substr(dot + 1, syntax.size());
    int index = atoi(s.c_str());
    return std::tuple<AttrType, std::string>{static_cast<AttrType>(index), syntax};
}

struct CLdapSchemaImpl
{
    std::unique_ptr<LDAPAttribute> oc{ new LDAPAttribute() };
    std::unique_ptr<LDAPAttribute> at{ new LDAPAttribute() };
    std::unique_ptr<LDAPAttribute> mr{ new LDAPAttribute() };

	std::map<std::string, std::tuple<AttrType, bool, std::string>, comp> attr2info;

	LDAPSchema classesSchema;
	LDAPSchema attributesSchema;

};

CLdapSchema::CLdapSchema()
	: m_impl(new CLdapSchemaImpl())
{

}

CLdapSchema::~CLdapSchema()
{

}

void CLdapSchema::build(LDAPConnection* lc, std::string& baseDn)
{
	StringList tmp;
	tmp.add("subschemasubentry");
    auto entries = lc->search("", LDAPConnection::SEARCH_BASE, "(objectClass=*)", tmp);
	uEntry rootDse(entries->getNext());
	std::string schemabase = "cn=subschema";
	if (rootDse)
	{
		const LDAPAttribute* schemaAttr = rootDse->getAttributes()->getAttributeByName("subschemaSubentry");
		schemabase = *(schemaAttr->getValues().begin());
	}

	StringList stringList;
	stringList.add("objectClasses");
	stringList.add("attributeTypes");
	stringList.add("matchingRules");
	entries = lc->search(schemabase, LDAPConnection::SEARCH_BASE, "(objectClass=*)", stringList);
	if (entries == 0)
	{
		return;
	}
	uEntry entry(entries->getNext());
	if (entry == 0)
	{
		return;
	}
	const LDAPAttributeList* attrs = entry->getAttributes();
	auto oc = attrs->getAttributeByName("objectClasses");
	auto at = attrs->getAttributeByName("attributeTypes");
	m_impl->oc.reset(new LDAPAttribute(*oc));
	m_impl->at.reset(new LDAPAttribute(*at));

	m_impl->attributesSchema.setAttributeTypes(at->getValues());
	m_impl->classesSchema.setObjectClasses(oc->getValues());

	QRegExp rxName1("NAME\\s+'([^']+)'");
	QRegExp rxName2("NAME\\s+\\(([^\\)]+)\\)");
	rxName1.setCaseSensitivity(Qt::CaseInsensitive);
	rxName2.setCaseSensitivity(Qt::CaseInsensitive);

	const StringList& atValues = m_impl->at->getValues();
	for (auto itr = atValues.begin(); itr != atValues.end(); ++itr)
	{
		QString text(itr->c_str());

		rxName1.indexIn(text);
		QStringList listName = rxName1.capturedTexts();
		QString name = listName[1];
		QStringList names;
        if (!name.isEmpty())
		{
			names << name;
		}
		else
		{
			rxName2.indexIn(text);
			listName = rxName2.capturedTexts();
			names = listName[1].replace("'", "").split(" ", QString::SkipEmptyParts);
		}

        auto r = FromAttributeString(*itr);
        bool isEditable = text.indexOf("NO-USER-MODIFICATION") == -1;

		for (const auto& i : names)
		{
            m_impl->attr2info[i.toStdString()] = std::tuple<AttrType, bool, std::string> {std::get<0>(r), isEditable, std::get<1>(r)};
		}

	}
}

std::tuple<AttrType, bool> CLdapSchema::GetAttributeInfoByName(std::string attrName)
{
	auto f = m_impl->attr2info.find(attrName);
	if (f != m_impl->attr2info.end())
	{
		return std::tuple<AttrType, bool>(std::get<0>(f->second), std::get<1>(f->second));
	}

	qDebug() << attrName.c_str() << '\n';
    return std::tuple<AttrType, bool> { AttrType::UnknownText, false };
}


bool CLdapSchema::isNameExist(std::string attributeName) throw (CLdapNameMissedException)
{
	auto f = m_impl->attr2info.find(attributeName);
	if (f == m_impl->attr2info.end())
	{
		QString error = QString("There is no arribute '%1' for this schema").arg(attributeName.c_str());
		throw CLdapNameMissedException(error.toStdString().c_str());
	}
	return true;
}

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

LDAPSchema* CLdapSchema::classesSchema()
{
	return &m_impl->classesSchema;
}

LDAPSchema* CLdapSchema::attributesSchema()
{
	return &m_impl->attributesSchema;
}

QVector<QString> CLdapSchema::classes()
{
    QVector<QString> r;
    for (auto& c : m_impl->classesSchema.object_classes)
    {
        r << c.second.getName().c_str();
    }
    return r;
}

QVector<QString> CLdapSchema::structuralClasses()
{
    QVector<QString> r;
    for (auto& c : m_impl->classesSchema.object_classes)
    {
        if (c.second.getKind() == 1)
        {
            r << c.second.getName().c_str();
        }
    }
    return r;
}

QVector<QString> CLdapSchema::auxiliaryClassesBySup(QString sup)
{
    QVector<QString> r;
    for (auto& c : m_impl->classesSchema.object_classes)
    {
        if (c.second.getKind() != 2)
        {
            continue;
        }
        if (c.second.getSup().contains(sup.toStdString()))
        {
            r << c.second.getName().c_str();
        }
    }
    return r;
}


QVector<CLdapAttribute> CLdapSchema::attributeByClasses(QVector<QString>& classes, std::map<std::string, std::string>& a2v)
{
    std::set<std::string> uniqueAttributes;
    QVector<CLdapAttribute> vector;
    for (auto& c : classes)
    {
        auto f = m_impl->classesSchema.getObjectClassByName(c.toStdString());
        auto name = f.getName();
        if (name.empty())
        {
            continue;
        }

        for (auto may : f.getMay())
        {
            auto attributeName = may;
            if (uniqueAttributes.find(attributeName) != uniqueAttributes.end())
            {
                continue;
            }
            uniqueAttributes.insert(attributeName);
            auto v2set = a2v.find(attributeName);
            auto v = v2set == a2v.end() ? "" : v2set->second;
            auto info = GetAttributeInfoByName(attributeName);
            auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
            CLdapAttribute a(attributeName.c_str(), v.c_str(), std::get<0>(info), false, attr.getDesc().c_str(),
                             v.empty() ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly);
            vector.push_back(a);
        }

        for (auto must : f.getMust())
        {
            auto attributeName = must;
            if (uniqueAttributes.find(attributeName) != uniqueAttributes.end())
            {
                continue;
            }
            uniqueAttributes.insert(attributeName);
            auto v2set = a2v.find(attributeName);
            auto v = v2set == a2v.end() ? "" : v2set->second;
            auto info = GetAttributeInfoByName(attributeName);
            auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
            CLdapAttribute a(attributeName.c_str(), v.c_str(), std::get<0>(info), true, attr.getDesc().c_str(),
                             v.empty() ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly);
            vector.push_back(a);
        }
    }
    return vector;

}

QString CLdapSchema::supByClass(QString c)
{
    const auto& f = classesSchema()->getObjectClassByName(c.toStdString());
    return f.getSup().size() ? f.getSup().begin()->c_str() : "";
}

QString CLdapSchema::startRdn(QString c)
{
    const auto& f = classesSchema()->getObjectClassByName(c.toStdString());
    auto must = std::move(f.getMust());
    return must.size() ? must.begin()->c_str() : "";
}

}

