#include <map>
#include <QRegExp>
#include "CLdapSchema.h"
#include "LDAPAttribute.h"
#include "LDAPConnection.h"
#include "LDAPEntry.h"

using uEntry = std::shared_ptr<LDAPEntry>;

namespace ldapcore
{

struct CLdapSchemaImpl
{
    std::unique_ptr<LDAPAttribute> oc;
    std::unique_ptr<LDAPAttribute> at;
    std::unique_ptr<LDAPAttribute> mr;

    std::map<std::string, std::tuple<AttrType, bool> > attr2info;

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
    m_impl->oc.reset(new LDAPAttribute(*attrs->getAttributeByName("objectClasses")));
    m_impl->at.reset(new LDAPAttribute(*attrs->getAttributeByName("attributeTypes")));
    m_impl->mr.reset(new LDAPAttribute(*attrs->getAttributeByName("matchingRules")));

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

        QRegExp rx("SYNTAX\\s+([0-9\\.]+)\\.([0-9]+)");
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        rx.indexIn(text);
        QStringList list = rx.capturedTexts();
        bool isEditable = text.indexOf("NO-USER-MODIFICATION") == -1;
        int syntaxIndex = list.length() == 3 && list[2].length() != 0 ? list[2].toInt() : 0;

        for (const auto& i : names  )
        {
            m_impl->attr2info[i.toStdString()] = std::tuple<AttrType, bool>{static_cast<AttrType>(syntaxIndex), isEditable};
        }

    }

}

std::tuple<AttrType, bool> CLdapSchema::GetAttributeInfoByName(std::string attrName)
{
    auto f = m_impl->attr2info.find(attrName);
    if (f != m_impl->attr2info.end())
    {
        return f->second;
    }
    return std::tuple<AttrType, bool>{AttrType::UnknownText, false};

}

}

