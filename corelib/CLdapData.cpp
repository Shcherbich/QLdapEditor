#include <CLdapData.h>
#include "qfunctional.h"
#include <LDAPConnection.h>
#include "StringList.h"

namespace ldapcore
{

CLdapData::CLdapData(QObject* parent)
    :QObject(parent)
{

}

CLdapData::~CLdapData()
{

}

void CLdapData::Connect(const tConnectionOptions& connectOptions)
{

    QThreadPool::globalInstance()->start(makeSimpleTask([=]
    {
        //std::this_thread::sleep_for (std::chrono::seconds(5));

        std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(connectOptions.host, connectOptions.port));
        try
        {
            if(connectOptions.simpleAuth)
            {
                if(connectOptions.useAnonymous)
                {
                    localConn->bind();
                }
                else
                {
                    //localConn->bind(connectOptions.basedn, connectOptions.password);
                    throw LDAPException(LDAP_AUTH_UNKNOWN, "This Authorize schema is not supported by LdApEditor application yet");
                }
            }
            else
            {
                throw LDAPException(LDAP_AUTH_UNKNOWN, "This Authorize schema is not supported by LdApEditor application yet");
            }

            m_baseDN = connectOptions.basedn;
            m_Connection = std::move(localConn);
            emit OnConnectionCompleted(this, true, QString(""));
        }
        catch(const LDAPException& e)
        {
            emit OnConnectionCompleted(this, false, QString(e.what()));
        }
    }));
}

void CLdapData::Disconnect()
{
    m_baseDN.clear();
    m_Connection.reset(nullptr);
}


QStringList CLdapData::GetTopObjectsList()
{
    tSearchOptions searchOptions;

    searchOptions.scope = LDAPAsynConnection::SEARCH_ONE;
    searchOptions.basedn = m_baseDN;
    //searchOptions.filter = "";
    //searchOptions.attributes = "";

    return Search(searchOptions);

//    if (m_Objects.size())
//    {
//        return m_Objects;
//    }
//    QVector<CLdapObject*> vector;
//    vector << new CLdapObject("Id_1", this);
//    vector << new CLdapObject("Id_2", this);
//    vector << new CLdapObject("Id_3", this);
//    m_Objects.append(vector);
//    return vector;
}

void CLdapData::AddObject(QString id)
{
    CLdapObject* added = new CLdapObject(id, this);
    QThreadPool::globalInstance()->start(makeSimpleTask([=]
    {
        std::this_thread::sleep_for (std::chrono::seconds(2));
        m_Objects << added;
        emit OnObjectAdded(added, true, QString(""));
    }));

}

void CLdapData::DeleteObject(CLdapObject* p)
{
    auto f = std::find_if(m_Objects.begin(), m_Objects.end(), [=](const CLdapObject* pItr) { return p == pItr; });
    if (f == m_Objects.end())
    {
        emit OnObjectDeleted(p, false, QString("No found!"));
    }
    else
    {
        QThreadPool::globalInstance()->start(makeSimpleTask([=]
        {
            std::this_thread::sleep_for (std::chrono::seconds(1));
            m_Objects.erase(f);
            emit OnObjectDeleted(p, true, QString(""));
        }));
    }
}

QString CLdapData::host()
{
    return QString::fromStdString(m_Connection->getHost());
}

int CLdapData::port()
{
    m_Connection->getPort();
}

QString CLdapData::baseDN()
{
    return QString::fromStdString(m_baseDN);
}

QStringList CLdapData::Search(const _tSearchOptions& searchOptions)
{
    QStringList objList;
    StringList attrList;

    try
    {
        for (QString& a : QString::fromStdString(searchOptions.attributes).split(","))
        {
            attrList.add(a.toStdString());
        }

        LDAPSearchResults* results =  m_Connection->search(searchOptions.basedn, searchOptions.scope,searchOptions.filter, attrList);
        if(results)
        {
            LDAPEntry* entry{nullptr};
            while(entry = results->getNext())
            {
                objList << QString::fromStdString(entry->getDN());
            }
        }
    }
    catch(const LDAPException& e)
    {

    }
    return objList;
}
}
