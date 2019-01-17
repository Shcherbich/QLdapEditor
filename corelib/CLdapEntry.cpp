#include "CLdapData.h"
#include "CLdapEntry.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"

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
    if(m_pEntry)
    {
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
    for(int i=1;i<=10;i++)
    {
        CLdapAttribute attr(QString("attr_%1").arg(i), QString("new_value"), AttrType::UnknownText, AttributeState::AttributeReadWrite);
        attributes.push_back(attr);
    }
    return attributes;
}

}

