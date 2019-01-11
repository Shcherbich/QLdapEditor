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
        rdn.trimmed();

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
            rdn.trimmed();
        }
        m_rDn = rdn;


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

QVector<CLdapAttribute> CLdapEntry::attributes()
{
    QVector<CLdapAttribute> ret;
    if(m_pEntry)
    {
        const LDAPAttributeList *al = m_pEntry->getAttributes();
        LDAPAttributeList::const_iterator i = al->begin();
        for ( ; i != al->end(); i++ )
        {
            std::string value;
            StringList values = i->getValues();
            StringList::const_iterator j = values.begin();
            for( ; j != values.end(); j++)
            {
                if (!value.empty())
                    value += ";";
                value += j->c_str();
            }


            if(value == "tesla")
            {
                CLdapAttribute attr(i->getName().c_str(), "TRUE", AttrType::Boolean);
                //CLdapAttribute attr(i->getName().c_str(), "2017-11-23", AttrType::Date);
                //CLdapAttribute attr(i->getName().c_str(), "23:05:58", AttrType::Time);
                ret.push_back(attr);
            }
//            else if(value == "88888")
//            {
//                CLdapAttribute attr(i->getName().c_str(), value.c_str(), AttrType::Int);
//                ret.push_back(attr);
//            }
            else if(value == "99999")
            {
                CLdapAttribute attr(i->getName().c_str(), "111111111222222223333333334444444", AttrType::Binary);
                ret.push_back(attr);
            }
            else
            {
                auto t = m_pData->schema().GetAttributeInfoByName(i->getName().c_str());

                auto tp = std::get<0>(t);
                //auto editable = std::get<1>(t);
                bool editable = g_supportedTypesForEdit.contains(tp);
                CLdapAttribute attr(i->getName().c_str(), value.c_str(), tp, editable);
                ret.push_back(attr);
            }

        }
    }
    return ret;
}

}

