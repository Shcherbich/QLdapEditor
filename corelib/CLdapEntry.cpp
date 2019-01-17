#include <string>
#include <vector>
#include <tuple>
#include "CLdapData.h"
#include "CLdapEntry.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"
#include "LDAPSchema.h"

std::vector<std::string> GetObjectClasses(LDAPConnection* le, std::string dn)
{
        std::vector<std::string> vRet;
        try
        {
                StringList stringList;
                stringList.add("objectClass");
                LDAPSearchResults* finds = le->search(dn, LDAPConnection::SEARCH_BASE, "(objectClass=*)", stringList);
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
                        vMay.push_back(may);
                for (const auto& must : ldapObjClass.getMust())
                        vMust.push_back(must);
        }
        return std::make_tuple(vMust, vMay);
}



namespace ldapcore
{
// supported types for edit
static QVector<ldapcore::AttrType> g_supportedTypesForEdit({
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

CLdapEntry::CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject *parent)
    : QObject(parent)
    , m_pParent(parentLdapEntry), m_pEntry(le)
{}

CLdapEntry::~CLdapEntry()
{
    delete m_pEntry;
    foreach (CLdapEntry* en, m_pEntries)
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

        if(rdn.isEmpty())
        {
            rdn = dn();
        }
        else
        {
            if(rdn.right(1) == ',')
            {
                rdn = rdn.left(rdn.length()-1);
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
    m_Conn = conn;
    try
    {
        LDAPSearchResults* ls = conn->search(dn().toStdString(), LDAPAsynConnection::SEARCH_ONE);
        if (ls != nullptr)
        {
            for (LDAPEntry* le = ls->getNext(); le != nullptr; le = ls->getNext())
            {
                m_pEntries.push_back(new CLdapEntry(this, le, nullptr));
                m_pEntries.back()->construct(data, conn, baseDn);
            }
        }
    }
    catch(LDAPException& ex)
    {

    }
}

CLdapEntry* CLdapEntry::parent()
{
    return m_pParent;
}

QVector<CLdapEntry*> CLdapEntry::children()
{
    return m_pEntries;
}

void CLdapEntry::prepareAttributes()
{
    m_attributes.clear();
    if(!m_pEntry)
    {
        return;
    }
    const LDAPAttributeList *al = m_pEntry->getAttributes();
    LDAPAttributeList::const_iterator i = al->begin();
    for ( ; i != al->end(); i++ )
    {
        auto t = m_pData->schema().GetAttributeInfoByName(i->getName().c_str());
        auto tp = std::get<0>(t);
        //auto editable = std::get<1>(t);
        AttributeState editState = g_supportedTypesForEdit.contains(tp) ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly;
        CLdapAttribute attr(i->getName().c_str(), i->toString().c_str(), tp, editState);
        m_attributes.push_back(attr);
    }

    {
        static StringList systemAttrs;
        if (systemAttrs.empty())
        {
            systemAttrs.add("creatorsname");
            systemAttrs.add("createtimestamp");
            systemAttrs.add("modifiersname");
            systemAttrs.add("structuralObjectClass");
            systemAttrs.add("entryUUID");
            systemAttrs.add("modifytimestamp");
            systemAttrs.add("subschemaSubentry");
            systemAttrs.add("hasSubordinates");
            systemAttrs.add("+");
            try
            {   LDAPSearchResults* srSystemAttrs = m_Conn->search(dn().toStdString(), LDAPConnection::SEARCH_SUB, "(objectClass=*)", systemAttrs);
                if (srSystemAttrs != nullptr)
                {
                        LDAPEntry* systemEntry = srSystemAttrs->top();
                        if (systemEntry != nullptr)
                        {
                                auto systemAttributes = systemEntry->getAttributes();
                                for (auto i = systemAttributes->begin(); i != systemAttributes->end(); ++i)
                                {
                                    auto t = m_pData->schema().GetAttributeInfoByName(i->getName().c_str());
                                    auto tp = std::get<0>(t);
                                    //auto editable = std::get<1>(t);
                                    AttributeState editState = g_supportedTypesForEdit.contains(tp) ? AttributeState::AttributeValueReadWrite : AttributeState::AttributeReadOnly;
                                    CLdapAttribute attr(i->getName().c_str(), i->toString().c_str(), tp, editState);
                                    m_attributes.push_back(attr);

                                }
                        }
                }

            }
            catch(std::exception e)
            {

            }
        }
    }
}

QVector<CLdapAttribute>* CLdapEntry::attributes()
{
    if(m_attributes.empty())
        prepareAttributes();
    return &m_attributes;
}

QVector<CLdapAttribute> CLdapEntry::availableAttributes()
{
    QVector<CLdapAttribute> attributes;
    auto av = GetAvailableAttributes(*m_pData->schema().classesSchema(), m_Conn, dn().toStdString());
    for (const auto& must : std::get<0>(av))
    {
        auto t = m_pData->schema().GetAttributeInfoByName(must);
        auto tp = std::get<0>(t);
        CLdapAttribute attr(must.c_str(), "", tp, AttributeState::AttributeReadWrite);
        attributes.push_back(attr);
    }
    for (const auto& may : std::get<1>(av))
    {
        auto t = m_pData->schema().GetAttributeInfoByName(may);
        auto tp = std::get<0>(t);
        CLdapAttribute attr(may.c_str(), "", tp, AttributeState::AttributeReadWrite);
        attributes.push_back(attr);
    }
    return attributes;
}

}

