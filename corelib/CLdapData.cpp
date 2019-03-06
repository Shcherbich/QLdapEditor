#include <CLdapData.h>
#include "qfunctional.h"
#include <LDAPConnection.h>
#include "StringList.h"
#include "common.h"

#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <stdio.h>
#include <syslog.h>

namespace ldapcore
{

using namespace std;

CLdapData::CLdapData(QObject* parent)
	: QObject(parent)
{

}

CLdapData::~CLdapData()
{
	resetConnection();
}

void CLdapData::syslogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context)
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stdout, "debug: %s\n", localMsg.constData());
        syslog(LOG_DEBUG, "debug: %s", localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stdout, "info: %s\n", localMsg.constData());
        syslog(LOG_INFO, "info: %s", localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stdout, "warning: %s\n", localMsg.constData());
        syslog(LOG_WARNING, "warning: %s", localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "critical: %s\n", localMsg.constData());
        syslog(LOG_CRIT, "critical: %s", localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "alert: %s\n", localMsg.constData());
        syslog(LOG_ALERT, "alert: %s", localMsg.constData());
    }
}

void CLdapData::initialize()
{
    static bool isInitialized = false;
    if (isInitialized)
    {
        return;
    }
    isInitialized = true;
    QtMessageHandler handler = CLdapData::syslogMessageHandler;
    qInstallMessageHandler(handler);

}

void CLdapData::connect(const tConnectionOptions& connectOptions)
{
    initialize();
	resetConnection();
    auto func = [&]()
    {
        try
        {
            qWarning() << "Connecting to " << connectOptions.host.c_str();
            std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(connectOptions.host, connectOptions.port));
            if (connectOptions.useTLS)
            {
                auto tls = localConn->getTlsOptions();
                if (connectOptions.cacertfile.size())
                {
                    tls.setOption(TlsOptions::CACERTFILE, connectOptions.cacertfile);
                }
                if (connectOptions.certfile.size())
                {
                    tls.setOption(TlsOptions::CERTFILE, connectOptions.certfile);
                }
                if (connectOptions.keyfile.size())
                {
                    tls.setOption(TlsOptions::KEYFILE, connectOptions.keyfile);
                }
                localConn->start_tls([&](std::string err)
                {
                    QString warningText = QString("<br>The LDAP Server uses an invalid certificate:</br><br><font color='#FF0000'>Description: %2</font></br><br></br><br>Do you wich proceed?</br>").arg(err.c_str());
                    m_CanUseUntrustedConnection = QMessageBox::warning(0, "Certificate trust", warningText, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes;
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
            m_connectOptions = connectOptions;
            m_Connection = std::move(localConn);
            m_Connection->setHardResetFunc([&]() { return hardReconnect(); });
            m_Schema.build(m_Connection.get());
            build();
            emit this->onConnectionCompleted(true, "");
            qWarning() << "Successfully connected to " << connectOptions.host.c_str();
        }
        catch (const LDAPException& e)
        {
            qCritical() << "Connection error to " << connectOptions.host.c_str() << ". " << e.what();
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
    auto en = new CLdapEntry(nullptr, nullptr, nullptr);
    en->initialize(this, m_baseDN.c_str());
    en->construct();
    m_Entries << en;
}

bool CLdapData::hardReconnect()
{
    try
    {
        m_Connection->init(m_connectOptions.host, m_connectOptions.port);
        if (m_connectOptions.useTLS)
        {
            auto tls = m_Connection->getTlsOptions();
            if (m_connectOptions.cacertfile.size())
            {
                tls.setOption(TlsOptions::CACERTFILE, m_connectOptions.cacertfile);
            }
            if (m_connectOptions.certfile.size())
            {
                tls.setOption(TlsOptions::CERTFILE, m_connectOptions.certfile);
            }
            if (m_connectOptions.keyfile.size())
            {
                tls.setOption(TlsOptions::KEYFILE, m_connectOptions.keyfile);
            }
            m_Connection->start_tls([&](std::string)
            {
                return m_CanUseUntrustedConnection;
            });
        }
        if (m_connectOptions.useAnonymous)
        {
            m_Connection->bind();
        }
        else
        {
            m_Connection->bind(m_connectOptions.username, m_connectOptions.password);
        }
        return true;
    }
    catch (const LDAPException& ex)
    {
        Q_UNUSED(ex);
    }
    return false;
}

void CLdapData::reconnect()
{
    std::unique_ptr<LDAPConnection> localConn(new LDAPConnection(m_connectOptions.host, m_connectOptions.port));
    if (m_connectOptions.useTLS)
    {
        auto tls = localConn->getTlsOptions();
        if (m_connectOptions.cacertfile.size())
        {
            tls.setOption(TlsOptions::CACERTFILE, m_connectOptions.cacertfile);
        }
        if (m_connectOptions.certfile.size())
        {
            tls.setOption(TlsOptions::CERTFILE, m_connectOptions.certfile);
        }
        if (m_connectOptions.keyfile.size())
        {
            tls.setOption(TlsOptions::KEYFILE, m_connectOptions.keyfile);
        }
        localConn->start_tls([&](std::string)
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
    m_Connection->setHardResetFunc([&]() { return hardReconnect(); });
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

CLdapServer& CLdapData::server()
{
    return m_Server;
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

bool CLdapData::isDnExist(QString dn)
{
    try
    {
        QString filter = QString(tr("%1")).arg(dn);
        auto results = m_Connection->search(dn.toStdString(), LDAPConnection::SEARCH_BASE);
        if (results != nullptr)
        {
            std::unique_ptr<LDAPEntry> entry(results->getNext());
            return entry.get() != nullptr;
        }
    }
    catch (const LDAPException& e)
    {
        Q_UNUSED(e);
    }
    return false;
}

void CLdapData::changeUserPassword(CLdapEntry* entry, const QString& userDN, const QString& newPassword)
{
    std::unique_ptr<LDAPModList> mods(new LDAPModList());
    std::string dn = userDN.toStdString();
    std::u16string wStr = QString("\"%1\"").arg(newPassword).toStdU16String();
    QByteArray bytes = QByteArray::fromRawData(reinterpret_cast<const char*>(wStr.c_str()), wStr.length() * sizeof(char16_t));
    std::string pwdData = bytes.toStdString();

    try
    {
        mods->addModification(LDAPModification(LDAPAttribute("unicodePwd", pwdData), LDAPModification::OP_REPLACE));
        entry->connectionPtr()->modify_s(dn, mods.get());
    }
    catch(const LDAPException& e)
    {
        QString msg =QString("%1\n%2").arg(e.getServerMsg().c_str()).arg(e.what());
        qCritical() << QString(tr("Failed to change password for user '%1': %2")).arg(userDN).arg(msg);
        throw CLdapServerException(msg.toStdString().c_str());
    }
    catch(const std::exception& e)
    {
        QString msg =QString("%1").arg(e.what());
        qCritical() << QString(tr("Failed to change password for user '%1': %2")).arg(userDN).arg(msg);
        throw CLdapServerException(msg.toStdString().c_str());
    }
}

}
