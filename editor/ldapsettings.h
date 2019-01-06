#ifndef LDAPSETTINGS_H
#define LDAPSETTINGS_H

#include "simplecrypt.h"
#include <QSettings>

namespace ldapeditor
{
    class CLdapSettings : protected QSettings
    {
    public:
        CLdapSettings(QObject* parent = Q_NULLPTR);

        QString name() const;
        void setName(const QString& name);

        QString host() const;
        void setHost(const QString& host);

        QString baseDN() const;
        void setBaseDN(const QString& baseDN);

        int port() const;
        void setPort(int port);

        int version() const;
        void setVersion(int version);

        bool simpleAuth() const;
        void setSimpleAuth(bool auth);

        bool useSSL() const;
        void setUseSSL(bool ssl);

        bool useTLS() const;
        void setUseTLS(bool tls);

        bool useSASL() const;
        void setUseSASL(bool sasl);

        QString username() const;
        void setUsername(const QString& username);

        QString password() const;
        void setPassword(const QString& password);

        bool useAnonymous() const;
        void setUseAnonymous(bool anonymous);

        void sync();

    protected:
        QString makeKey(const QString& key) const;
         QString readString(const QString& key, const QString& defaultValue="") const;
         void writeString(const QString& key, const QString& value);

         int readInt(const QString& key, int defaultValue=0) const;
         void writeInt(const QString& key, int value);

         bool readBool(const QString& key, bool defaultValue=false) const;
         void writeBool(const QString& key, bool value);

         mutable SimpleCrypt m_crypto;

    };
}//namespace ldapeditor


#endif // LDAPSETTINGS_H
