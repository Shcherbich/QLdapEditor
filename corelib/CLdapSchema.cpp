#include <map>
#include <set>
#include <algorithm>
#include <functional>
#include <regex>
#include <tuple>
#include <QRegExp>
#include <QDebug>
#include "CLdapAttribute.h"
#include "CLdapSchema.h"
#include "LDAPConnection.h"
#include "LDAPEntry.h"
#include "datatypes.h"
#include "datatypesinternal.h"
#include <string.h>
#include <string>
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
			return std::tuple<AttrType, std::string> {f->second, syntax};
		}
		return std::tuple<AttrType, std::string> {AttrType::UnknownText, ""};
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
			return std::tuple<AttrType, std::string> {f->second, syntax};
		}
		return std::tuple<AttrType, std::string> {AttrType::UnknownText, ""};
	}

	std::size_t dot = syntax.find_last_of(".");
	auto s = syntax.substr(dot + 1, syntax.size());
	int index = atoi(s.c_str());
	return std::tuple<AttrType, std::string> {static_cast<AttrType>(index), syntax};
}


CLdapSchema::CLdapSchema()
	: m_impl(new CLdapSchemaImpl())
{

}

CLdapSchema::~CLdapSchema()
{

}

void CLdapSchema::build(LDAPConnection* lc)
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

std::tuple<AttrType, bool> CLdapSchema::attributeInfoByName(std::string attrName)
{
	auto f = m_impl->attr2info.find(attrName);
	if (f != m_impl->attr2info.end())
	{
		return std::tuple<AttrType, bool>(std::get<0>(f->second), std::get<1>(f->second));
	}
	return std::tuple<AttrType, bool> { AttrType::UnknownText, false };
}


bool CLdapSchema::isNameExist(std::string attributeName) noexcept(false)
{
	auto f = m_impl->attr2info.find(attributeName);
	if (f == m_impl->attr2info.end())
	{
		QString error = QString("There is no arribute '%1' for this schema").arg(attributeName.c_str());
		throw CLdapNameMissedException(error.toStdString().c_str());
	}
	return true;
}


LDAPSchema* CLdapSchema::classesSchema()
{
	return &m_impl->classesSchema;
}

LDAPSchema* CLdapSchema::attributesSchema()
{
	return &m_impl->attributesSchema;
}

QStringList CLdapSchema::classes()
{
	QStringList r;
	for (auto& c : m_impl->classesSchema.object_classes)
	{
		r << c.second.getName().c_str();
	}
	return r;
}

QStringList CLdapSchema::structuralClasses()
{
	QStringList r;
	for (auto& c : m_impl->classesSchema.object_classes)
	{
		if (c.second.getKind() == 1)
		{
			r << c.second.getName().c_str();
		}
	}
	return r;
}

QStringList CLdapSchema::auxiliaryClasses()
{
    QStringList r;
    for (auto& c : m_impl->classesSchema.object_classes)
    {
        if (c.second.getKind() == 2)
        {
            r << c.second.getName().c_str();
        }
    }
    return r;
}

QStringList CLdapSchema::auxiliaryClassesBySup(QString sup)
{
	QStringList r;
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


QVector<CLdapAttribute> CLdapSchema::attributeByClasses(QStringList& classes, std::map<std::string, std::string>& a2v)
{
    static std::set<std::string> excludedAttributeNames;
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
            if (uniqueAttributes.find(attributeName) != uniqueAttributes.end() ||
                excludedAttributeNames.find(attributeName) != excludedAttributeNames.end())
			{
				continue;
			}
			uniqueAttributes.insert(attributeName);
			auto v2set = a2v.find(attributeName);
			auto v = v2set == a2v.end() ? "" : v2set->second;
			auto info = attributeInfoByName(attributeName);
			auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
			QStringList classes;
			CLdapAttribute a(attributeName.c_str(), v.c_str(), std::get<0>(info), false, attr.getDesc().c_str(), classes,
			                 v.empty() ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly);
			vector.push_back(a);
		}

		for (auto must : f.getMust())
		{
			auto attributeName = must;
            if (uniqueAttributes.find(attributeName) != uniqueAttributes.end() ||
                excludedAttributeNames.find(attributeName) != excludedAttributeNames.end())
			{
				continue;
			}
			uniqueAttributes.insert(attributeName);
			auto v2set = a2v.find(attributeName);
			auto v = v2set == a2v.end() ? "" : v2set->second;
			auto info = attributeInfoByName(attributeName);
			auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
			QStringList classes;
			CLdapAttribute a(attributeName.c_str(), v.c_str(), std::get<0>(info), true, attr.getDesc().c_str(), classes,
			                 v.empty() ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly);
			vector.push_back(a);
		}
	}
	return vector;

}

QStringList CLdapSchema::classesByAttributeName(std::string attrName, QStringList& classesOfEntry)
{
	QStringList classes;
    for (auto& c : classesOfEntry)
    {
        auto classByName = m_impl->classesSchema.getObjectClassByName(c.toStdString());
        if (classByName.getMay().contains(attrName) || classByName.getMust().contains(attrName))
        {
            classes << c;
        }
    }
    return classes;
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
    return must.size() ? must.begin()->c_str() : "CN";
}

QString CLdapSchema::classDescription(const QString& cls)
{
	LDAPObjClass objectClassByName = m_impl->classesSchema.getObjectClassByName(cls.toStdString());
	std::string desc = objectClassByName.getDesc();
	return QString(QObject::tr("%1 class")).arg(desc.empty() ? cls : desc.c_str());
}

QVector<CLdapAttribute> CLdapSchema::mayAttributesByClass(QString cl)
{
    QVector<CLdapAttribute> vector;
    LDAPObjClass ldapObjClass = classesSchema()->getObjectClassByName(cl.toStdString());
    for (const auto& attributeName: ldapObjClass.getMay())
    {
        auto info = attributeInfoByName(attributeName);
        auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
        QStringList classes;
        CLdapAttribute a(attributeName.c_str(), "", std::get<0>(info), false, attr.getDesc().c_str(), classes,
                         AttributeState::AttributeValueReadWrite);
        vector.push_back(a);
    }
    return vector;
}

QVector<CLdapAttribute> CLdapSchema::mustAttributesByClass(QString cl)
{
    QVector<CLdapAttribute> vector;
    LDAPObjClass ldapObjClass = classesSchema()->getObjectClassByName(cl.toStdString());
    for (const auto& attributeName: ldapObjClass.getMust())
    {
        auto info = attributeInfoByName(attributeName);
        auto attr = m_impl->attributesSchema.getAttributeTypeByName(attributeName);
        QStringList classes;
        CLdapAttribute a(attributeName.c_str(), "", std::get<0>(info), true, attr.getDesc().c_str(), classes,
                         AttributeState::AttributeValueReadWrite);
        vector.push_back(a);
    }
    return vector;
}


QStringList CLdapSchema::consistentClassesByStructuralAndOther(QString structuralCl, QStringList allClasses)
{
    QSet<QString> uniqueClasses;
    QStringList result;
    for (;;)
    {
        if (!uniqueClasses.contains(structuralCl))
        {
            uniqueClasses << structuralCl;
            result << structuralCl;
        }
        auto sup = supByClass(structuralCl);
        if (sup.isEmpty() || sup == structuralCl)
        {
            break;
        }
        structuralCl = sup;
    }
    for (auto& c: allClasses)
    {
        if (!uniqueClasses.contains(c))
        {
            result << c;
        }
    }
    return result;
}

QString CLdapSchema::deductStructuralClass(const QStringList &classes)
{
    QStringList structuralClasses;
    for (auto& c: classes)
    {
        if (classesSchema()->getObjectClassByName(c.toStdString()).getKind() != 2)
            structuralClasses << c;
    }
    QStringList consistentClasses, tmpList;
    for (auto& sc: structuralClasses)
    {
        auto vector = consistentClassesByStructuralAndOther(sc, tmpList);
        if (vector.size() > consistentClasses.size())
        {
            consistentClasses.swap(vector);
        }
    }
    return consistentClasses.size() ? consistentClasses.first() : "";
}


}

