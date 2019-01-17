#include <map>
#include <algorithm>
#include <functional>
#include <QRegExp>
#include <QDebug>
#include "CLdapSchema.h"
#include "LDAPAttribute.h"
#include "LDAPConnection.h"
#include "LDAPEntry.h"
#include "LDAPSchema.h"
#include <string.h>
#include <string>

struct comp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

using uEntry = std::shared_ptr<LDAPEntry>;

namespace ldapcore
{

struct CLdapSchemaImpl
{
    std::unique_ptr<LDAPAttribute> oc;
    std::unique_ptr<LDAPAttribute> at;
    std::unique_ptr<LDAPAttribute> mr;

    std::map<std::string, std::tuple<AttrType, bool, std::string> , comp> attr2info;

    LDAPSchema classesSchema;
    LDAPSchema attributesSchema;

};

CLdapSchema::CLdapSchema()
    :m_impl(new CLdapSchemaImpl())
{

}

CLdapSchema::~CLdapSchema()
{

}

void CLdapSchema::build(LDAPConnection* lc, std::string& baseDn)
{
    StringList tmp;
    tmp.add("subschemasubentry");
    LDAPSearchResults* entries = lc->search("", LDAPConnection::SEARCH_BASE, "(objectClass=*)", tmp);
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
    //m_impl->mr.reset(new LDAPAttribute(*attrs->getAttributeByName("matchingRules")));

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
        if (name.length() != 0)
        {
            names << name;
        }
        else
        {
            rxName2.indexIn(text);
            listName = rxName2.capturedTexts();
            names = listName[1].replace("'", "").split(" ", QString::SkipEmptyParts);
        }

        QRegExp rx("SYNTAX\\s+'*([0-9\\.]+)\\.([0-9]+)'*");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        rx.indexIn(text);
        QStringList list = rx.capturedTexts();
        bool isEditable = text.indexOf("NO-USER-MODIFICATION") == -1;
        int syntaxIndex = list.length() == 3 && list[2].length() != 0 ? list[2].toInt() : 0;
        std::string syntax = list.length() == 3 && list[2].length() != 0 ? list[1].toStdString() + "." + list[2].toStdString() : "";

        for (const auto& i : names  )
        {
            qDebug() << name << " " << syntax.c_str() << " " << syntaxIndex << '\n';

            m_impl->attr2info[i.toStdString()] = std::tuple<AttrType, bool, std::string>{static_cast<AttrType>(syntaxIndex), isEditable, syntax};
        }

    }

}

std::tuple<AttrType, bool> CLdapSchema::GetAttributeInfoByName(std::string attrName)
{
    auto f = m_impl->attr2info.find(attrName);
    if (f != m_impl->attr2info.end())
    {
        qDebug() << "  Found  " << attrName.c_str() << '\n';
        return std::tuple<AttrType, bool>(std::get<0>(f->second), std::get<1>(f->second));
    }

    qDebug() << attrName.c_str() << '\n';

    return std::tuple<AttrType, bool>{AttrType::UnknownText, false};

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
        if (!b1 && !b2)
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
    auto f = syntaxNumber2toCheckFunction.find(attributeName);
    if (f != syntaxNumber2toCheckFunction.end())
    {
        auto checkReturn = f->second(value);
        if (checkReturn.size() != 0)
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


}

