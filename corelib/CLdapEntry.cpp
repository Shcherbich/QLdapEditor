#include "CLdapEntry.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"


namespace ldapcore
{

CLdapEntry::CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject *parent)
    : m_pParent(parentLdapEntry), m_pEntry(le), QObject(parent)
{}

CLdapEntry::~CLdapEntry()
{
    delete m_pEntry;
    foreach (CLdapEntry* en, m_pEntries)
    {
        delete en;
    }
}

QString CLdapEntry::getDn()
{
    return m_pEntry->getDN().c_str();
}

void CLdapEntry::construct(LDAPConnection* conn)
{
    try
    {
        LDAPSearchResults* ls = conn->search(getDn().toStdString(), LDAPAsynConnection::SEARCH_ONE);
        if (ls != nullptr)
        {
            for (LDAPEntry* le = ls->getNext(); le != nullptr; le = ls->getNext())
            {
                m_pEntries.push_back(new CLdapEntry(this, le, nullptr));
                m_pEntries.back()->construct(conn);
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

QVector<CLdapAttribute> CLdapEntry::attributes()
{
    QVector<CLdapAttribute> ret;

    const LDAPAttributeList *al = m_pEntry->getAttributes();
    LDAPAttributeList::const_iterator i = al->begin();
    for ( ; i != al->end(); i++ )
    {
        StringList values = i->getValues();
        StringList::const_iterator j = values.begin();
        for( ; j != values.end(); j++)
        {
        }

        CLdapAttribute attr(i->getName().c_str(), j->c_str(), AttrType::Int);
        ret.push_back(attr);
    }
    return ret;
}

}
