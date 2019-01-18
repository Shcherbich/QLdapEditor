#include "ldapsettings.h"
#include "ldapeditordefines.h"


namespace ldapeditor
{
    CLdapSettings::CLdapSettings(QObject* parent):
        QSettings(QSettings::IniFormat, QSettings::UserScope, OrganizationName, ApplicationName, parent)
      , m_crypto(Q_UINT64_C(0x208babde2123560e))
    {

    }
    QString CLdapSettings::name() const
    {
        return readString("name");
    }

    void CLdapSettings::setName(const QString& name)
    {
        writeString("name", name);
    }

    QString CLdapSettings::host() const
    {
        return readString("host", "localhost");
    }
    void CLdapSettings::setHost(const QString& host)
    {
        writeString("host", host);
    }

    QString CLdapSettings::baseDN() const
    {
        return readString("baseDN");
    }
    void CLdapSettings::setBaseDN(const QString& baseDN)
    {
        writeString("baseDN", baseDN);
    }

    int CLdapSettings::port() const
    {
        return readInt("port", 389);
    }
    void CLdapSettings::setPort(int port)
    {
        writeInt("port", port);
    }

    int CLdapSettings::version() const
    {
        return readInt("version", 3);
    }
    void CLdapSettings::setVersion(int version)
    {
         writeInt("version", version);
    }

    bool CLdapSettings::simpleAuth() const
    {
        return readBool("simpleAuth", true);
    }
    void CLdapSettings::setSimpleAuth(bool auth)
    {
        writeBool("simpleAuth",auth);
    }

    bool CLdapSettings::useSSL() const
    {
        return readBool("useSSL");
    }
    void CLdapSettings::setUseSSL(bool ssl)
    {
        writeBool("useSSL",ssl);
    }

    bool CLdapSettings::useTLS() const
    {
         return readBool("useTLS");
    }

    void CLdapSettings::setUseTLS(bool tls)
    {
         writeBool("useTLS",tls);
    }

    bool CLdapSettings::useSASL() const
    {
        return readBool("useSASL");
    }

    void CLdapSettings::setUseSASL(bool sasl)
    {
        writeBool("useSASL",sasl);
    }

    QString CLdapSettings::username() const
    {
        //return readString("username");
        QString sE = readString("username");
        return m_crypto.decryptToString(sE);
    }

    void CLdapSettings::setUsername(const QString& username)
    {
        //writeString("username", username);
        QString sE = m_crypto.encryptToString(username);
        writeString("username",sE);
    }

    QString CLdapSettings::password() const
    {
        //return readString("password");
        QString sE = readString("password");
        return m_crypto.decryptToString(sE);
    }

    void CLdapSettings::setPassword(const QString& password)
    {
         //writeString("password",password);
        QString sE = m_crypto.encryptToString(password);
        writeString("password", sE);
    }

    bool CLdapSettings::useAnonymous() const
    {
        return readBool("useAnonymous");
    }
    void CLdapSettings::setUseAnonymous(bool anonymous)
    {
       writeBool("useAnonymous",anonymous);
    }

    int CLdapSettings::timeout() const
    {
        return readInt("timeout", 15);
    }

    void CLdapSettings::sync()
    {
        QSettings::sync();
    }
    int CLdapSettings::columnDn() const
    {
        return readInt("columnDn", 60);
    }
    void CLdapSettings::setColumnDn(int w)
    {
        return writeInt("columnDn", w);
    }

    int CLdapSettings::columnAttribute() const
    {
        return readInt("columnAttribute", 60);
    }
    void CLdapSettings::setColumnAttribute(int w)
    {
        return writeInt("columnAttribute", w);
    }

    int CLdapSettings::columnValue() const
    {
        return readInt("columnValue", 60);
    }
    void CLdapSettings::setColumnValue(int w)
    {
        return writeInt("columnValue", w);
    }

    int CLdapSettings::columnType() const
    {
        return readInt("columnType", 60);
    }
    void CLdapSettings::setColumnType(int w)
    {
        return writeInt("columnType", w);
    }

    int CLdapSettings::columnSize() const
    {
        return readInt("columnSize", 60);
    }
    void CLdapSettings::setColumnSize(int w)
    {
        return writeInt("columnSize", w);
    }

    QString CLdapSettings::makeKey(const QString& key) const
    {
        return QString("connection/%1").arg(key);
    }

    QString CLdapSettings::readString(const QString& key, const QString& defaultValue) const
    {
        return value(makeKey(key), defaultValue).toString();
    }
    void CLdapSettings::writeString(const QString& key, const QString& value)
    {
        setValue(makeKey(key), value);
    }

    int CLdapSettings::readInt(const QString& key, int defaultValue) const
    {
        return value(makeKey(key), defaultValue).toInt();
    }
    void CLdapSettings::writeInt(const QString& key, int value)
    {
        setValue(makeKey(key), value);
    }

    bool CLdapSettings::readBool(const QString& key, bool defaultValue) const
    {
        return value(makeKey(key), defaultValue).toBool();
    }
    void CLdapSettings::writeBool(const QString& key, bool value)
    {
        setValue(makeKey(key), value);
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
