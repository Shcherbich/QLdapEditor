#include <CLdapData.h>
#include "qfunctional.h"
#include <LDAPConnection.h>
#include "StringList.h"
#include <vector>
#include <string>
#include <QMessageBox>
#include <QString>

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
    auto func = [&]()
    {
        try
        {
            std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(connectOptions.host, connectOptions.port));
            if (connectOptions.useTLS)
            {
                localConn->start_tls([&](std::string err)
                {
                    QString warningText = QString("<br>The LDAP Server uses an invalid certificate:</br><br><font color='#FF0000'>Description: %2</font></br><br></br><br>Do you wich proceed?</br>").arg(err.c_str());
                    m_CanUseUntrustedConnection = QMessageBox::warning(0, "Certificate trust", warningText, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes;
                    return m_CanUseUntrustedConnection;
                });
            }
            if (connectOptions.useAnonymous)
            {
                localConn->bind();
            }
            else
            {
                localConn->bind(connectOptions.username, connectOptions.password);
            }
            m_baseDN = connectOptions.basedn;
            m_Connection = std::move(localConn);
            m_connectOptions = connectOptions;
            m_Schema.build(m_Connection.get(), m_baseDN);
            build();
            emit this->onConnectionCompleted(true, "");
        }
        catch (const LDAPException& e)
        {
            emit this->onConnectionCompleted(false, e.what());
        }
    };
    auto task = makeSimpleTask(func);
    task->setAutoDelete(false);
    QThreadPool::globalInstance()->start(task);
}

void CLdapData::rebuild()
{
	foreach (CLdapEntry* en, m_Entries)
	{
		delete en;
	}
	m_Entries.clear();
	build();
}

void CLdapData::build()
{
	if (m_Connection.get() == nullptr)
	{
		return;
	}
    m_baseDN = QString(m_baseDN.c_str()).trimmed().toStdString();
	m_Entries.push_back(new CLdapEntry(nullptr, nullptr, nullptr));
	m_Entries.back()->construct(this, m_Connection.get(), m_baseDN.c_str());
}

void CLdapData::reconnect()
{
    std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(m_connectOptions.host, m_connectOptions.port));
    if (m_connectOptions.useTLS)
    {
        localConn->start_tls([&](std::string err)
        {
            return m_CanUseUntrustedConnection;
        });
    }
    if (m_connectOptions.useAnonymous)
    {
        localConn->bind();
    }
    else
    {
        localConn->bind(m_connectOptions.username, m_connectOptions.password);
    }
    m_Connection = std::move(localConn);

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

        auto results = m_Connection->search(searchOptions.basedn, searchOptions.scope, searchOptions.filter, attrList);
        if (results != nullptr)
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
