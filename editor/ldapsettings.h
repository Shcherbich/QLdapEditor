/*!
@file
@brief Header file for application settings class

File contains  declarations for application settings class
*/
#ifndef LDAPSETTINGS_H
#define LDAPSETTINGS_H

#include "simplecrypt.h"
#include "datatypes.h"

#include <QSettings>

namespace ldapeditor
{
    /*!
     * @ingroup ldapeditor
     * @brief Application settings class
     *
     * Attribute helper class provides save/restore Application settings functionality
     *
     */
    class CLdapSettings : protected QSettings
    {
    public:
        /*!
         * \brief Constructor CLdapSettings
         * \param settingsFile path to settings
         * \param parent pointer to parent QObject
         */
        CLdapSettings(QString configFile, QObject* parent = nullptr);

//        /*!
//         * \brief Constructor CLdapSettings
//         * \param parent pointer to parent QObject
//         */
//        CLdapSettings(QObject* parent = nullptr);

        /*!
         * \brief Method returns connection name
         * \return connection name or empty string
         */
        QString name() const;

        /*!
         * \brief Method sets connection name
         * \param name - connection name string
         */
        void setName(const QString& name);

        /*!
         * \brief Method returns connection host
         * \return connection host or empty string
         */
        QString host() const;

        /*!
         * \brief Method sets connection host
         * \param host - connection host string
         */
        void setHost(const QString& host);

        /*!
         * \brief Method returns base DN
         * \return connection host or empty string
         */
        QString baseDN() const;

        /*!
         * \brief Method sets connection base DN
         * \param baseDN - connection base DN string
         */
        void setBaseDN(const QString& baseDN);

        /*!
         * \brief Method returns port number
         * \return connection port number
         */
        int port() const;

        /*!
         * \brief Method sets connection port number
         * \param port - connection port number
         */
        void setPort(int port);

        /*!
         * \brief Method returns version of LDAP protocol
         * \return connection version of LDAP protocol number
         */
        int version() const;

        /*!
         * \brief Method sets version of LDAP protocol number
         * \param version - version of LDAP protocol number
         */
        void setVersion(int version);

        /*!
         * \brief Method returns is set Simple authentification
         * \return true if simple authentification set
         */
        bool simpleAuth() const;

        /*!
         * \brief Method sets using simple authentification
         * \param auth - use or not simple authentification
         */
        void setSimpleAuth(bool auth);

        /*!
         * \brief Method returns is set using SSL for authentification
         * \return true if set using SSL for authentification
         */
        bool useSSL() const;

        /*!
         * \brief Method sets using SSL for authentification
         * \param ssl - use or not SSL for authentification
         */
        void setUseSSL(bool ssl);

        /*!
         * \brief Method returns is set using TLS for authentification
         * \return true if set using TLS for authentification
         */
        bool useTLS() const;

        /*!
         * \brief Method sets using TLS for authentification
         * \param tls - use or not TLS for authentification
         */
        void setUseTLS(bool tls);

        /*!
         * \brief Method returns is set using SASL for authentification
         * \return true if set using SASL for authentification
         */
        bool useSASL() const;

        /*!
         * \brief Method sets using SASL for authentification
         * \param sasl - use or not SASL for authentification
         */
        void setUseSASL(bool sasl);

        /*!
         * \brief Method returns decrypted username
         * \return connection username or empty string
         */
        QString username() const;

        /*!
         * \brief Method sets connection username
         * \param username - connection username string
         */
        void setUsername(const QString& username);

        /*!
         * \brief Method returns descrypted password
         * \return connection username or empty string
         */
        QString password() const;

        /*!
         * \brief Method sets connection password
         * \param password - connection password string
         */
        void setPassword(const QString& password);

        /*!
         * \brief Method returns is set using anonymous connection
         * \return true if set using anonymous connection
         */
        bool useAnonymous() const;

        /*!
         * \brief Method sets using anonymous connection
         * \param anonymous - use or not anonymous connection
         */
        void setUseAnonymous(bool anonymous);

        /*!
         * \brief Method returns width of 'Ignore' column in atrributes list
         * \return  width of column
         */
        int columnIgnore() const;

        /*!
         * \brief Method sets width of 'Ignore' column in attributes list
         * \param w - new width of column
         */
        void setColumnIgnore(int w);

        /*!
         * \brief Method returns width of 'Name' column in atrributes list
         * \return  width of column
         */
        int columnDn() const;

        /*!
         * \brief Method sets width of 'Name' column in attributes list
         * \param w - new width of column
         */
        void setColumnDn(int w);

        /*!
         * \brief Method returns width of 'Class' column in atrributes list
         * \return  width of column
         */
        int columnClass() const;

        /*!
         * \brief Method sets width of 'Class' column in attributes list
         * \param w - new width of column
         */
        void setColumnClass(int w);

        /*!
         * \brief Method returns width of 'Attribute' column in atrributes list
         * \return  width of column
         */
        int columnAttribute() const;

        /*!
         * \brief Method sets width of 'Attribute' column in attributes list
         * \param w - new width of column
         */
        void setColumnAttribute(int w);

        /*!
         * \brief Method returns width of 'Value' column in atrributes list
         * \return  width of column
         */
        int columnValue() const;

        /*!
         * \brief Method sets width of 'Value' column in attributes list
         * \param w - new width of column
         */
        void setColumnValue(int w);

        /*!
         * \brief Method returns width of 'Type' column in atrributes list
         * \return  width of column
         */
        int columnType() const;

        /*!
         * \brief Method sets width of 'Type' column in attributes list
         * \param w - new width of column
         */
        void setColumnType(int w);

        /*!
         * \brief Method returns width of 'Size' column in atrributes list
         * \return  width of column
         */
        int columnSize() const;

        /*!
         * \brief Method sets width of 'Size' column in attributes list
         * \param w - new width of column
         */
        void setColumnSize(int w);

        /*!
         * \brief Method returns timeout settings
         * \return number  timeoue
         */
        int timeout() const;

        /*!
         * \brief Method saves data with external storage/file
         */
        void saveSettings();

        /*!
         * \brief Method returns reference to connection Options
         * \return connection options
         */
        ldapcore::tConnectionOptions& connectionOptions()const;

        /*!
         * \brief Method provides base DN string for find users
         * \return return string with base DN where to find users
         */
        QString findUsersBaseDN() const;

        /*!
         * \brief Method provides filter string for find users
         * \return return string with filter to find users
         */
        QString     findUsersFilter() const;

    protected:
        /*!
         * \brief protected method for makeKey
         * \param key settings key
         * \param section settings section
         * \return string section/key
         */
        QString makeKey(const QString& key, const QString& section) const;

        /*!
          * \brief protected method reads string data
          * \param key settings key
          * \param section settings section
          * \param defaultValue default value, used if key is not found
          * \return string value
          */
         QString readString(const QString& key, const QString& section, const QString& defaultValue="") const;

         /*!
          * \brief protected method writes string data
          * \param key settings key
          * \param section settings section
          * \param value data for saving
          */
         void writeString(const QString& key, const QString& section, const QString& value);

         /*!
          * \brief protected method reads integer data
          * \param key settings key
          * \param section settings section
          * \param defaultValue default value, used if key is not found
          * \return integer value
          */
         int readInt(const QString& key, const QString& section, int defaultValue=0) const;

         /*!
          * \brief protected method writes integer data
          * \param key settings key
          * \param section settings section
          * \param value data for saving
          */
          void writeInt(const QString& key, const QString& section, int value);

          /*!
           * \brief protected method reads boolean data
           * \param key settings key
           * \param section settings section
           * \param defaultValue default value, used if key is not found
           * \return boolean value
           */
         bool readBool(const QString& key, const QString& section, bool defaultValue=false) const;

         /*!
          * \brief protected method writes boolean data
          * \param key settings key
          * \param section settings section
          * \param value data for saving
          */
         void writeBool(const QString& key, const QString& section, bool value);

         mutable SimpleCrypt m_crypto; ///< simple engine for crype/decrypt data
         mutable ldapcore::tConnectionOptions m_connectionOptions; ///< connection options

         QString m_configFile;

    };
}//namespace ldapeditor


#endif // LDAPSETTINGS_H
