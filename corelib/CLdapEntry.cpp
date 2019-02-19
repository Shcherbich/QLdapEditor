#include <string>
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <QDebug>
#include <QThread>
#include "CLdapData.h"
#include "CLdapEntry.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"
#include "LDAPModification.h"
#include "LDAPSchema.h"
#include "LDAPMessage.h"

static StringList systemAttrs;
using uEntry = std::shared_ptr<LDAPEntry>;

struct comp
{
    bool operator()(const ldapcore::CLdapAttribute& l, const ldapcore::CLdapAttribute& r)
    {
        if (l.isMust() != r.isMust())
        {
            return l.isMust() > r.isMust();
        }
        return l.name() < r.name();
    }
};

std::vector<std::string> GetObjectClasses(LDAPConnection* le, std::string dn)
{
    std::vector<std::string> vRet;
    try
    {
        StringList stringList;
        stringList.add("objectClass");
        auto finds = le->search(dn, LDAPConnection::SEARCH_BASE, "(objectClass=*)", stringList);
        if (finds == nullptr)
        {
            return vRet;
        }
        uEntry en(finds->getNext());
        if (en == nullptr)
        {
            return vRet;
        }
        const LDAPAttributeList* attrs = en->getAttributes();
        if (attrs == nullptr)
        {
            return vRet;
        }
        auto objectClasses = attrs->getAttributeByName("objectClass");
        if (objectClasses == nullptr)
        {
            return vRet;
        }
        for (const auto& cl : objectClasses->getValues())
        {
            vRet.push_back(cl);
        }
    }
    catch (std::exception& e)
    {
    }
    return vRet;
}

std::tuple< std::vector<std::string>, std::vector<std::string> >  GetAvailableAttributes(LDAPSchema& classSchema, LDAPConnection* le, std::string dn)
{
    std::vector<std::string> classes = GetObjectClasses(le, dn);
    std::vector<std::string> vMay;
    std::vector<std::string> vMust;
    for (const auto& cl : classes)
    {
        LDAPObjClass ldapObjClass = classSchema.getObjectClassByName(cl);
        for (const auto& may : ldapObjClass.getMay())
        {
            vMay.push_back(may);
        }
        for (const auto& must : ldapObjClass.getMust())
        {
            vMust.push_back(must);
        }
    }
    return std::make_tuple(vMust, vMay);
}

namespace ldapcore
{
// supported types for edit
static QVector<ldapcore::AttrType> g_supportedTypesForEdit(
{
    ldapcore::AttrType::UnknownText,
    ldapcore::AttrType::Binary,
    ldapcore::AttrType::Boolean,
    ldapcore::AttrType::DN,
    ldapcore::AttrType::GeneralizedTime,
    ldapcore::AttrType::IA5String,
    ldapcore::AttrType::Integer,
    ldapcore::AttrType::Oid,
    ldapcore::AttrType::OctetString,
    ldapcore::AttrType::TelephoneNumber
});

CLdapEntry::CLdapEntry(CLdapEntry* parentLdapEntry, QString rdn, QString parentDn, QVector<QString>& classes, QObject* parent)
    : QObject(parent)
    , m_pParent(parentLdapEntry)
{
    m_rDn = rdn;
    QString s = rdn;
    s += ",";
    s += parentDn;
    m_pEntry = new LDAPEntry(s.toStdString());
    m_classes.swap(classes);
    m_isNew = true;
}

CLdapEntry::CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject* parent)
    : QObject(parent)
    , m_pParent(parentLdapEntry), m_pEntry(le)
{}

CLdapEntry::~CLdapEntry()
{
    delete m_pEntry;
    foreach (CLdapEntry* en, m_vChildren)
    {
        delete en;
    }
}

QString CLdapEntry::dn()
{
    return m_pEntry ? m_pEntry->getDN().c_str() : m_baseDn;
}

QString CLdapEntry::rDn()
{
    if (m_rDn.isEmpty())
    {
        QString rdn(dn());
        rdn.replace(parent() ? parent()->dn() : m_baseDn, "");
        rdn = rdn.trimmed();

        if (rdn.isEmpty())
        {
            rdn = dn();
        }
        else
        {
            if (rdn.right(1) == ',')
            {
                rdn = rdn.left(rdn.length() - 1);
            }
            rdn = rdn;
        }
        m_rDn = rdn.trimmed();
    }
    return m_rDn;
}

void CLdapEntry::initialize(CLdapData* data, QString baseDn)
{
    m_pData = data;
    m_baseDn = baseDn;
}

void CLdapEntry::construct(CLdapData* data, QString baseDn)
{
    initialize(data, baseDn);
    try
    {
        auto ls = connectionPtr()->search(dn().toStdString(), LDAPAsynConnection::SEARCH_ONE);
        if (ls)
        {
            for (LDAPEntry* le = ls->getNext(); le; le = ls->getNext())
            {
                auto en = new CLdapEntry(this, le, nullptr);
                en->construct(data, baseDn);
                m_vChildren << en;
            }
        }
    }
    catch (const LDAPException& ex)
    {

    }
}

LDAPConnection* CLdapEntry::connectionPtr() const
{
    return m_pData->m_Connection.get();
}

CLdapEntry* CLdapEntry::parent()
{
    return m_pParent;
}

QVector<CLdapEntry*> CLdapEntry::children()
{
    return m_vChildren;
}

void CLdapEntry::prepareAttributes()
{
    if (m_isNew)
    {
        return;
    }

    m_attributes.clear();
    if (!m_pEntry)
    {
        return;
    }

    availableAttributesMayImpl();
    availableAttributesMustImpl();

    loadAttributes(m_attributes);

}
void CLdapEntry::loadAttributes(QVector<CLdapAttribute>& vRet, bool needToLoadSystemAttributes)
{
    if (!m_pEntry || m_isNew)
    {
        return;
    }
    const LDAPAttributeList* al = m_pEntry->getAttributes();
    const LDAPAttribute* pObjectClass = al->getAttributeByName("objectClass");
    if (pObjectClass && !m_classes.size())
    {
        QVector<QString> classes;
        for (const auto& cl : pObjectClass->getValues())
        {
            classes.push_back(cl.c_str());
        }
        setClasses(classes);
    }
    LDAPAttributeList::const_iterator i = al->begin();
    for (; i != al->end(); i++)
    {
        auto t = m_pData->schema().attributeInfoByName(i->getName().c_str());
        auto tp = std::get<0>(t);
        auto editable = std::get<1>(t);
        AttributeState editState = editable ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly;
        auto name = i->getName();
        auto attributeTypeByName = m_pData->schema().attributesSchema()->getAttributeTypeByName(name);
        auto attrClasses = m_pData->schema().classesByAttributeName(name, m_classes);
        CLdapAttribute attr(name.c_str(), i->toString().c_str(), tp, isMust(name), attributeTypeByName.getDesc().c_str(), attrClasses, (isMust(name) && !isNew()) ? AttributeState::AttributeReadOnly : editState);
        vRet.push_back(attr);
    }


    {
        if (systemAttrs.empty())
        {
            systemAttrs.add("creatorsname");
            systemAttrs.add("createtimestamp");
            systemAttrs.add("modifiersname");
            systemAttrs.add("objectClasses");
            systemAttrs.add("entryUUID");
            systemAttrs.add("modifytimestamp");
            systemAttrs.add("subschemaSubentry");
            systemAttrs.add("hasSubordinates");
            systemAttrs.add("+");
        }

        try
        {
            auto srSystemAttrs = m_pData->m_Connection->search(dn().toStdString(), LDAPConnection::SEARCH_SUB, "(objectClass=*)", systemAttrs);
            if (srSystemAttrs != nullptr && needToLoadSystemAttributes)
            {
                LDAPEntry* systemEntry = srSystemAttrs->top();
                if (systemEntry != nullptr)
                {
                    auto systemAttributes = systemEntry->getAttributes();
                    for (auto i = systemAttributes->begin(); i != systemAttributes->end(); ++i)
                    {
                        auto t = m_pData->schema().attributeInfoByName(i->getName().c_str());
                        auto tp = std::get<0>(t);
                        auto name = i->getName();
                        auto attributeTypeByName = m_pData->schema().attributesSchema()->getAttributeTypeByName(name);
                        auto attrClasses = m_pData->schema().classesByAttributeName(name, m_classes);
                        CLdapAttribute attr(name.c_str(), i->toString().c_str(), tp, isMust(name), attributeTypeByName.getDesc().c_str(), attrClasses, AttributeState::AttributeReadOnly);
                        vRet.push_back(attr);
                        m_Must.push_back(attr);
                    }
                }
            }

        }
        catch (std::exception e)
        {

        }

    }

    std::sort(vRet.begin(), vRet.end(), comp());

}

QVector<CLdapAttribute>* CLdapEntry::attributes()
{
    if (m_attributes.empty())
    {
        prepareAttributes();
    }
    return &m_attributes;
}

QVector<CLdapAttribute> CLdapEntry::availableAttributesMust()
{
    return m_Must;
}

QVector<CLdapAttribute> CLdapEntry::availableAttributesMay()
{
    return m_May;
}


void CLdapEntry::availableAttributesMustImpl()
{
    QVector<CLdapAttribute>tmp;
    m_Must.swap(tmp);
    auto av = GetAvailableAttributes(*m_pData->schema().classesSchema(), connectionPtr(), dn().toStdString());
    for (const auto& must : std::get<0>(av))
    {
        auto t = m_pData->schema().attributeInfoByName(must);
        auto tp = std::get<0>(t);
        auto attributeTypeByName = m_pData->schema().attributesSchema()->getAttributeTypeByName(must.c_str());
        QVector<QString> classes;
        CLdapAttribute attr(must.c_str(), "", tp, true, attributeTypeByName.getDesc().c_str(), classes, AttributeState::AttributeReadWrite);
        m_Must.push_back(attr);
    }
}

void CLdapEntry::availableAttributesMayImpl()
{
    QVector<CLdapAttribute>tmp;
    m_May.swap(tmp);
    auto av = GetAvailableAttributes(*m_pData->schema().classesSchema(), connectionPtr(), dn().toStdString());
    for (const auto& may : std::get<1>(av))
    {
        auto t = m_pData->schema().attributeInfoByName(may);
        auto tp = std::get<0>(t);
        auto attributeTypeByName = m_pData->schema().attributesSchema()->getAttributeTypeByName(may.c_str());
        QVector<QString> classes;
        CLdapAttribute attr(may.c_str(), "", tp, false, attributeTypeByName.getDesc().c_str(), classes, AttributeState::AttributeReadWrite);
        m_May.push_back(attr);
    }
}

std::shared_ptr<CLdapAttribute> CLdapEntry::createEmptyAttribute(std::string attributeName)
{
    auto t = m_pData->schema().attributeInfoByName(attributeName);
    auto tp = std::get<0>(t);
    QVector<QString> classes;
    std::shared_ptr<CLdapAttribute> p(new CLdapAttribute(attributeName.c_str(), "", tp, isMust(attributeName), "", classes, AttributeState::AttributeReadWrite));
    return p;
}

bool CLdapEntry::isMust(std::string attributeName)
{
    auto f = std::find_if(m_Must.begin(), m_Must.end(), [&](const ldapcore::CLdapAttribute & a)
    {
        return a.name() == attributeName.c_str();
    });
    return f != m_Must.end();
}

bool CLdapEntry::isNew()
{
    return m_isNew;
}

bool CLdapEntry::isEdit()
{
    return m_isEdit;
}

void CLdapEntry::setEditable(bool isEdit)
{
    m_isEdit = isEdit;
}


void CLdapEntry::validateAttribute(CLdapAttribute& attr)
{
    m_pData->schema().isNameExist(attr.name().toStdString());
    m_pData->schema().validateAttributeByName(attr.name().toStdString(), attr.value().toStdString());
}

void CLdapEntry::flushAttributeCache()
{
    auto sr = connectionPtr()->search(m_pEntry->getDN(), LDAPConnection::SEARCH_BASE, "(objectClass=*)");
    if (sr)
    {
        std::shared_ptr<LDAPEntry> entry(sr->getNext());
        if (entry.get() != nullptr)
        {
            *m_pEntry = *entry.get();
            prepareAttributes();
        }
    }
}

void CLdapEntry::sortAttributes()
{
    std::sort(m_attributes.begin(), m_attributes.end(), comp());
}

QVector<QString> CLdapEntry::classes()
{
    return m_classes;
}

QVector<QString> CLdapEntry::auxiliaryClasses()
{
    QVector<QString> vector;
    auto auxiliaryClasses = m_pData->schema().auxiliaryClasses();
    for (auto& cl : m_classes)
    {
        if (auxiliaryClasses.contains(cl))
        {
            vector << cl;
        }
    }
    return vector;
}

void CLdapEntry::setClasses(QVector<QString>& cList)
{
    m_classes.clear();
    m_classes << cList;
}

QVector<QString> CLdapEntry::availableClasses()
{
    return m_pData->schema().classes();
}

void CLdapEntry::addChild(CLdapEntry* child)
{
    child->initialize(m_pData, m_baseDn);
    m_vChildren << child;
}

void CLdapEntry::removeChild(CLdapEntry* child)
{
    for (int i = 0; i < m_vChildren.size(); ++i)
    {
        if (m_vChildren[i] == child)
        {
            //Known issue: delete child;
            m_vChildren.remove(i);
            return;
        }
    }
}


void CLdapEntry::addAttributes(QVector<CLdapAttribute>& attrs)
{
    m_attributes << attrs;
    std::sort(m_attributes.begin(), m_attributes.end(), comp());
}

QString  CLdapEntry::structuralClass()
{
    return m_pData->schema().deductStructuralClass(m_classes);
}

}

