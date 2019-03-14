/*!
@file
@brief Implemenattion file for application settings class

File contains  implementation for application settings class
*/

#include "ldapsettings.h"
#include "ldapeditordefines.h"
#include "ldapeditordefines.h"

namespace ldapeditor
{
    CLdapSettings::CLdapSettings(QString configFile, QObject* parent):
        QSettings(configFile, QSettings::IniFormat, parent)
      , m_crypto(Q_UINT64_C(0x208babde2123560e))
      , m_configFile{configFile}
    {

    }

//    CLdapSettings::CLdapSettings(QObject* parent):
//        QSettings(QSettings::IniFormat, QSettings::UserScope, QString(OrganizationName), QString(), parent)
//      , m_crypto(Q_UINT64_C(0x208babde2123560e))
//    {

//    }

    QString CLdapSettings::name() const
    {
        return readString("name","connection");
    }

    void CLdapSettings::setName(const QString& name)
    {
        writeString("name", "connection", name);
    }

    QString CLdapSettings::host() const
    {
        return readString("host", "connection",  "localhost");
    }
    void CLdapSettings::setHost(const QString& host)
    {
        writeString("host", "connection",  host);
    }

    QString CLdapSettings::baseDN() const
    {
        return readString("baseDN", "connection");
    }
    void CLdapSettings::setBaseDN(const QString& baseDN)
    {
        writeString("baseDN","connection",  baseDN);
    }

    int CLdapSettings::port() const
    {
        return readInt("port", "connection", 389);
    }
    void CLdapSettings::setPort(int port)
    {
        writeInt("port","connection",  port);
    }

    int CLdapSettings::version() const
    {
        return readInt("version","connection",  3);
    }
    void CLdapSettings::setVersion(int version)
    {
         writeInt("version", "connection", version);
    }

    bool CLdapSettings::simpleAuth() const
    {
        return readBool("simpleAuth", "connection", true);
    }
    void CLdapSettings::setSimpleAuth(bool auth)
    {
        writeBool("simpleAuth","connection", auth);
    }

    bool CLdapSettings::useSSL() const
    {
        return readBool("useSSL","connection");
    }
    void CLdapSettings::setUseSSL(bool ssl)
    {
        writeBool("useSSL","connection", ssl);
    }

    bool CLdapSettings::useTLS() const
    {
         return readBool("useTLS","connection");
    }

    void CLdapSettings::setUseTLS(bool tls)
    {
         writeBool("useTLS","connection", tls);
    }

    bool CLdapSettings::useSASL() const
    {
        return readBool("useSASL","connection");
    }

    void CLdapSettings::setUseSASL(bool sasl)
    {
        writeBool("useSASL","connection", sasl);
    }

    QString CLdapSettings::username() const
    {
        //return readString("username");
        QString sE = readString("username","connection");
        return m_crypto.decryptToString(sE);
    }

    void CLdapSettings::setUsername(const QString& username)
    {
        //writeString("username", username);
        QString sE = m_crypto.encryptToString(username);
        writeString("username","connection", sE);
    }

    QString CLdapSettings::password() const
    {
        //return readString("password");
        QString sE = readString("password","connection");
        return m_crypto.decryptToString(sE);
    }

    void CLdapSettings::setPassword(const QString& password)
    {
         //writeString("password",password);
        QString sE = m_crypto.encryptToString(password);
        writeString("password", "connection", sE);
    }

    bool CLdapSettings::useAnonymous() const
    {
        return readBool("useAnonymous","connection");
    }
    void CLdapSettings::setUseAnonymous(bool anonymous)
    {
       writeBool("useAnonymous","connection", anonymous);
    }

    int CLdapSettings::timeout() const
    {
        return readInt("timeout", "connection", 15);
    }

    void CLdapSettings::saveSettings()
    {
        QSettings::sync();
    }

    int CLdapSettings::columnIgnore() const
    {
        int r = readInt("columnIgnore", "attrList", 60);
        return r ? r : 100;
    }

    void CLdapSettings::setColumnIgnore(int w)
    {
        return writeInt("columnIgnore", "attrList",w);
    }

    int CLdapSettings::columnDn() const
    {
        int r = readInt("columnDn", "attrList", 60);
        return r ? r : 100;
    }

    void CLdapSettings::setColumnDn(int w)
    {
        return writeInt("columnDn", "attrList",w);
    }

    int CLdapSettings::columnClass() const
    {
        int r = readInt("columnClass", "attrList", 60);
        return r ? r : 100;
    }
    void CLdapSettings::setColumnClass(int w)
    {
        return writeInt("columnClass", "attrList",w);
    }

    int CLdapSettings::columnAttribute() const
    {
        int r = readInt("columnAttribute", "attrList", 60);
        return r ? r : 100;
    }
    void CLdapSettings::setColumnAttribute(int w)
    {
        return writeInt("columnAttribute", "attrList",w);
    }

    int CLdapSettings::columnValue() const
    {
        int r = readInt("columnValue", "attrList", 60);
        return r ? r : 100;
    }
    void CLdapSettings::setColumnValue(int w)
    {
        return writeInt("columnValue", "attrList",w);
    }

    int CLdapSettings::columnType() const
    {
        int r = readInt("columnType", "attrList", 60);
        return r ? r : 100;
    }
    void CLdapSettings::setColumnType(int w)
    {
        return writeInt("columnType", "attrList",w);
    }

    int CLdapSettings::columnSize() const
    {
        int r = readInt("columnSize", "attrList", 60);
        return r ? r : 100;
    }
    void CLdapSettings::setColumnSize(int w)
    {
        return writeInt("columnSize", "attrList",w);
    }


    QString CLdapSettings::findUsersBaseDN() const
    {
        return QString{"DC=sc4i,DC=local"};
    }

    QString     CLdapSettings::findUsersFilter() const
    {
        return QString{"objectClass=user"};
    }

    QString CLdapSettings::makeKey(const QString& key, const QString& section) const
    {
        return QString("%1/%2").arg(section).arg(key);
    }

    QString CLdapSettings::readString(const QString& key, const QString& section, const QString& defaultValue) const
    {
        return value(makeKey(key,section), defaultValue).toString();
    }
    void CLdapSettings::writeString(const QString& key, const QString& section, const QString& value)
    {
        setValue(makeKey(key, section), value);
    }

    int CLdapSettings::readInt(const QString& key, const QString& section, int defaultValue) const
    {
        return value(makeKey(key,section), defaultValue).toInt();
    }
    void CLdapSettings::writeInt(const QString& key, const QString& section, int value)
    {
        setValue(makeKey(key,section), value);
    }

    bool CLdapSettings::readBool(const QString& key, const QString& section, bool defaultValue) const
    {
        return value(makeKey(key,section), defaultValue).toBool();
    }
    void CLdapSettings::writeBool(const QString& key, const QString& section, bool value)
    {
        setValue(makeKey(key,section), value);
    }

    ldapcore::tConnectionOptions& CLdapSettings::connectionOptions()const
    {
        m_connectionOptions.host = host().toStdString();
        m_connectionOptions.port = port();
        m_connectionOptions.version = version();
        m_connectionOptions.basedn = baseDN().toStdString();
        m_connectionOptions.username = username().toStdString();
        m_connectionOptions.password = password().toStdString();
        m_connectionOptions.simpleAuth = simpleAuth();
        m_connectionOptions.useSSL = useSSL();
        m_connectionOptions.useTLS = useTLS();
        m_connectionOptions.useSASL = useSASL();
        m_connectionOptions.useAnonymous = useAnonymous();
        m_connectionOptions.timeout = timeout();
        return m_connectionOptions;
    }
}//namespace ldapeditor
