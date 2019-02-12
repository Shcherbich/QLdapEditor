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
        LDAPEntry* en = finds->getNext();
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

std::string AddAttributeToServer(LDAPConnection* conn, LDAPEntry* le, std::string name, std::string value)
{
    try
    {
        LDAPAttribute newattr(name);
        newattr.addValue(value);
        LDAPModification::mod_op op = LDAPModification::OP_ADD;
        LDAPModList* mod = new LDAPModList();
        mod->addModification(LDAPModification(newattr, op));
        conn->modify_s(le->getDN(), mod);
        return "";
    }
    catch (const std::exception& ex)
    {
        return ex.what();
    }
}

std::string DeleteAttributeFromServer(LDAPConnection* conn, LDAPEntry* le, std::string name)
{
    try
    {
        LDAPAttribute newattr(name);
        LDAPModification::mod_op op = LDAPModification::OP_DELETE;
        LDAPModList* mod = new LDAPModList();
        mod->addModification(LDAPModification(newattr, op));
        conn->modify_s(le->getDN(), mod);
        return "";
    }
    catch (const std::exception& ex)
    {
        return ex.what();
    }
}

std::string ReplaceAttributeOnServer(LDAPConnection* conn, LDAPEntry* le, std::string name, std::string value)
{
    try
    {
        LDAPModification::mod_op op = LDAPModification::OP_REPLACE;
        LDAPModList* mod = new LDAPModList();
        auto q = conn->search(le->getDN(), LDAPAsynConnection::SEARCH_SUB, "objectClass=*", StringList());
        auto en = q->getNext();
        auto find = en->getAttributeByName(name);
        if (find == nullptr)
        {
            throw ldapcore::CLdapServerException("No found attribute");
        }
        StringList newList;
        newList.add(value);
        const_cast<LDAPAttribute*>(find)->setValues(newList);
        mod->addModification(LDAPModification(*find, op));
        conn->modify_s(le->getDN(), mod);
        return "";
    }
    catch (const std::exception& ex)
    {
        return ex.what();
    }
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
    foreach (CLdapEntry* en, m_pChildren)
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

void CLdapEntry::construct(CLdapData* data, LDAPConnection* conn, QString baseDn)
{
    m_pData = data;
    m_baseDn = baseDn;
    try
    {
        auto ls = conn->search(dn().toStdString(), LDAPAsynConnection::SEARCH_ONE);
        if (ls)
        {
            for (LDAPEntry* le = ls->getNext(); le; le = ls->getNext())
            {
                m_pChildren.push_back(new CLdapEntry(this, le, nullptr));
                m_pChildren.back()->construct(data, conn, baseDn);
            }
        }
    }
    catch (LDAPException& ex)
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
    return m_pChildren;
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
    if (pObjectClass )
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
        CLdapAttribute attr(name.c_str(), i->toString().c_str(), tp, isMust(name), attributeTypeByName.getDesc().c_str(), attrClasses, editState);
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
                        CLdapAttribute attr(name.c_str(), i->toString().c_str(), tp, true, attributeTypeByName.getDesc().c_str(), attrClasses, AttributeState::AttributeReadOnly);
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

void CLdapEntry::addAttribute(CLdapAttribute& newOb) noexcept(false)
{
    auto ret = AddAttributeToServer(connectionPtr(), m_pEntry, newOb.name().toStdString(), newOb.value().toStdString());
    if (ret.size())
    {
        auto err = QString("Add new attribute '%1': %2").arg(newOb.name()).arg(ret.c_str());
        throw CLdapServerException(err.toStdString().c_str());
    }
}

void CLdapEntry::deleteAttribute(CLdapAttribute& object) noexcept(false)
{
    auto ret = DeleteAttributeFromServer(connectionPtr(), m_pEntry, object.name().toStdString());
    if (ret.size())
    {
        auto err = QString("Delete attribute '%1': %2").arg(object.name()).arg(ret.c_str());
        throw CLdapServerException(err.toStdString().c_str());
    }
}

void CLdapEntry::updateAttribute(CLdapAttribute& object) noexcept(false)
{
    auto ret = ReplaceAttributeOnServer(connectionPtr(), m_pEntry, object.name().toStdString(), object.value().toStdString());
    if (ret.size())
    {
        auto err = QString("Update attribute '%1': %2").arg(object.name()).arg(ret.c_str());
        throw CLdapServerException(err.toStdString().c_str());
    }
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

void CLdapEntry::setClasses(QVector<QString>& cList)
{
    m_classes.clear();
    m_classes << cList;
}

QVector<QString> CLdapEntry::availableClasses()
{
    return m_pData->schema().classes();
}

void CLdapEntry::addNewChild(CLdapEntry* child)
{
    m_pChildren << child;
    child->construct(m_pData, connectionPtr(), m_baseDn);
}

void CLdapEntry::removeChild(CLdapEntry* child)
{
    for (int i = 0; i < m_pChildren.size(); ++i)
    {
        if (m_pChildren[i] == child)
        {
            //Known issue: delete child;
            m_pChildren.remove(i);
            return;
        }
    }
}


void CLdapEntry::addAttributes(QVector<CLdapAttribute>& attrs)
{
    m_attributes << attrs;
    std::sort(m_attributes.begin(), m_attributes.end(), comp());
}

void CLdapEntry::saveNewChild() noexcept(false)
{
    auto f = std::find_if(m_pChildren.begin(), m_pChildren.end(),
                          [&](ldapcore::CLdapEntry * a)
    {
        return a->isNew();
    });
    if (m_pChildren.end() == f)
    {
        return;
    }
    auto& child = *f;
    try
    {
        std::shared_ptr<LDAPAttributeList> attrs(new LDAPAttributeList());
        StringList objectClasses;
        for (auto& c : child->m_classes)
        {
            if (c != "top")
            {
                objectClasses.add(c.toStdString());
            }
        }
        attrs->addAttribute(LDAPAttribute("objectClass", objectClasses));
        static std::set<QString> excludedAttributeNames {"cn"};

        for (auto& a : child->m_attributes)
        {
            auto value = a.value().toStdString();
            if (value.size())
            {
                attrs->addAttribute(LDAPAttribute(a.name().toStdString(), value));
            }
        }
        std::shared_ptr<LDAPEntry> entry(new LDAPEntry(child->m_pEntry->getDN(), attrs.get()));
        auto dn = m_pEntry->getDN();
        connectionPtr()->add(entry.get());
        child->m_isNew = false;
    }
    catch (const std::exception& ex)
    {
        throw CLdapServerException(ex.what());
    }
}

void CLdapEntry::update() noexcept(false)
{
    if (m_isEdit == false)
    {
        return;
    }
    try
    {
        QVector<CLdapAttribute> realAttributes;
        loadAttributes(realAttributes);

        LDAPModList* mod = new LDAPModList();

        // 1. Add/Modify attributes
        StringList objectClasses;
        for (auto& c : m_classes)
            objectClasses.add(c.toStdString());
        mod->addModification(LDAPModification(LDAPAttribute("objectClass", objectClasses), LDAPModification::OP_REPLACE));
        for (auto& a : m_attributes) {

            if (a.name() == "objectClass"){
                continue;
            }

            auto f = std::find_if(realAttributes.begin(), realAttributes.end(),
                [&](const ldapcore::CLdapAttribute& o) {
                    return strcasecmp(o.name().toStdString().c_str(), a.name().toStdString().c_str()) == 0;
                });

            auto value = a.value().toStdString();

            // no modification
            if (f != realAttributes.end() && f->value() == a.value()) {
                continue;
            }

            // add modification
            if (!value.empty())
            {
                if (f == realAttributes.end())
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_ADD));
                }
                else
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_REPLACE));
                }
            }
        }


        std::set<QString> setOfAttributes;
        for (auto& a: m_attributes)
        {
            setOfAttributes.insert(a.name());
        }

        // 2. Delete attributes
        auto new_end = std::remove_if(realAttributes.begin(), realAttributes.end(),
                                      [&](const ldapcore::CLdapAttribute& o)
                                      { return setOfAttributes.find(o.name()) != setOfAttributes.end() || !o.isMust() ; });
        realAttributes.erase(new_end, realAttributes.end());
        for (auto& a: realAttributes)
        {
            mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), a.value().toStdString()),
                                                  LDAPModification::OP_DELETE));
        }

        auto& dn = m_pEntry->getDN();
        connectionPtr()->modify_s(dn, mod);
        m_isEdit = false;
    }
    catch (const std::exception& ex)
    {
        throw CLdapServerException(ex.what());
    }
}

}

