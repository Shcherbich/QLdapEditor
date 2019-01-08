#include "CLdapEntry.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"


namespace ldapcore
{


CLdapEntry::CLdapEntry(QObject *parent)
    : QObject(parent)
{

}

CLdapEntry::CLdapEntry(LDAPEntry* le, QObject *parent)
    : QObject(parent), m_pEntry(le)
{}

CLdapEntry::~CLdapEntry()
{
    delete m_pEntry;
    foreach (CLdapEntry* en, m_pEntries)
    {
        delete en;
    }
}

QString CLdapEntry::GetDn()
{
    return m_pEntry->getDN().c_str();
}

void CLdapEntry::Construct(LDAPConnection* conn)
{
    try
    {
        LDAPSearchResults* ls = conn->search(GetDn().toStdString(), LDAPAsynConnection::SEARCH_ONE);
        if (ls != nullptr)
        {
            for (LDAPEntry* le = ls->getNext(); le != nullptr; le = ls->getNext())
            {
                m_pEntries.push_back(new CLdapEntry(le, nullptr));
                m_pEntries.back()->Construct(conn);
            }
        }
    }
    catch(LDAPException& ex)
    {

    }
}

QVector<CLdapEntry*> CLdapEntry::Children()
{
    return m_pEntries;
}

QVector<CLdapAttribute> CLdapEntry::Attributes()
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
