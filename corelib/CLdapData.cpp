#include <CLdapData.h>
#include "qfunctional.h"
#include <LDAPConnection.h>
#include "StringList.h"
#include <vector>
#include <string>

namespace ldapcore
{

using namespace std;
vector<string> split(const string& str, const string& delim)
{
	vector<string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == string::npos)
		{
			pos = str.length();
		}
		string token = str.substr(prev, pos - prev);
		if (!token.empty())
		{
			tokens.push_back(token);
		}
		prev = pos + delim.length();
	}
	while (pos < str.length() && prev < str.length());
	return tokens;
}

CLdapData::CLdapData(QObject* parent)
	: QObject(parent)
{

}

CLdapData::~CLdapData()
{
	resetConnection();
}

void CLdapData::connect(const tConnectionOptions& connectOptions)
{
	resetConnection();

	QThreadPool::globalInstance()->start(makeSimpleTask([ = ]
	{
		try
		{
			std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(connectOptions.host, connectOptions.port));
			if (connectOptions.simpleAuth)
			{
				if (connectOptions.useTLS)
				{
					std::string cefile("/etc/pki/tls/certs/ca-bundle.trust.crt");
					std::string cedir("/etc/pki/tls/certs");
					TlsOptions tls = localConn->getTlsOptions();
					tls.setOption(TlsOptions::REQUIRE_CERT, TlsOptions::DEMAND);
					tls.setOption(TlsOptions::CACERTFILE, cefile);
					localConn->start_tls();
				}
				else
				{
					if (connectOptions.useAnonymous)
					{
						localConn->bind();
					}
					else
					{
						localConn->bind(connectOptions.username, connectOptions.password);
					}
				}
			}
			else
			{
				throw LDAPException(LDAP_AUTH_UNKNOWN, "This Authorize schema is not supported by LdApEditor application yet");
			}

			m_baseDN = connectOptions.basedn;
			m_Connection = std::move(localConn);
			m_Schema.build(m_Connection.get(), m_baseDN);
			build();
			emit OnConnectionCompleted(this, true, QString(""));
		}
		catch (const LDAPException& e)
		{
			emit OnConnectionCompleted(this, false, QString(e.what()));
		}
	}));
}

void CLdapData::build()
{
	if (m_Connection.get() == nullptr)
	{
		return;
	}
	auto v = split(m_baseDN, ",");
	std::string baseDn;
	for (auto itr = v.begin(); itr != v.end(); ++itr)
	{
		auto str = *itr;
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
		if (str.find("dc=") == 0)
		{
			if (baseDn.size())
			{
				baseDn += ",";
			}
			baseDn += str;
		}
	}
	m_baseDN = baseDn;
	m_Entries.push_back(new CLdapEntry(nullptr, nullptr, nullptr));
	m_Entries.back()->construct(this, m_Connection.get(), m_baseDN.c_str());

	/*LDAPSearchResults* ls = m_Connection->search(m_baseDN, LDAPAsynConnection::SEARCH_ONE);
	if (ls != nullptr)
	{
	    for (LDAPEntry* le = ls->getNext(); le != nullptr; le = ls->getNext())
	    {
	        m_Entries.push_back(new CLdapEntry(nullptr, le, nullptr));
	        m_Entries.back()->construct(m_Connection.get(), m_baseDN);
	    }
	}*/
}

void CLdapData::resetConnection()
{
	foreach (CLdapEntry* en, m_Entries)
	{
		delete en;
	}
	m_Entries.clear();
	m_baseDN.clear();
	m_Connection.reset(nullptr);
}

QVector<CLdapEntry*> CLdapData::topList()
{
	return m_Entries;
}

void CLdapData::addObject(QString id)
{
	Q_UNUSED(id);
	/*
	CLdapObject* added = new CLdapObject(id, this);
	QThreadPool::globalInstance()->start(makeSimpleTask([=]
	{
	    std::this_thread::sleep_for (std::chrono::seconds(2));
	    m_Objects << added;
	    emit OnObjectAdded(added, true, QString(""));
	}));
	*/
}

void CLdapData::deleteObject(CLdapObject* p)
{
	Q_UNUSED(p);
	/*auto f = std::find_if(m_Objects.begin(), m_Objects.end(), [=](const CLdapObject* pItr) { return p == pItr; });
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
	*/
}

QString CLdapData::host()
{
	return QString::fromStdString(m_Connection->getHost());
}

int CLdapData::port()
{
	return m_Connection->getPort();
}

QString CLdapData::baseDN()
{
	return QString::fromStdString(m_baseDN);
}

CLdapSchema& CLdapData::schema()
{
	return m_Schema;
}

QStringList CLdapData::search(const _tSearchOptions& searchOptions)
{
	QStringList objList;
	StringList attrList;

	try
	{
		for (QString& a : QString::fromStdString(searchOptions.attributes).split(","))
		{
			attrList.add(a.toStdString());
		}

		LDAPSearchResults* results =  m_Connection->search(searchOptions.basedn, searchOptions.scope, searchOptions.filter, attrList);
		if (results)
		{
			LDAPEntry* entry{nullptr};
			while ((entry = results->getNext()) != nullptr)
			{
				objList << QString::fromStdString(entry->getDN());
			}
		}
	}
	catch (const LDAPException& e)
	{
		Q_UNUSED(e);
	}
	return objList;
}
}
