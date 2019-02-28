#ifndef CLdapData_H
#define CLdapData_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include "CLdapEntry.h"
#include "cldapserver.h"
#include "datatypes.h"
#include "CLdapSchema.h"
#include <memory>
#include <QtGlobal>
#include <QString>
#include <QByteArray>

class LDAPConnection;

namespace ldapcore
{

/*!
 * @ingroup ldapcore
 * @brief The ldap data class
 *
 * The ldap data is main back-end class for managing ldap server.
 * One instance of CLdapData is real-time enough.
 *
 */
class CLdapData : public QObject
{
	Q_OBJECT

public:

    /*!
     * \brief Constructor CLdapData by default
     */
	explicit CLdapData(QObject* parent = nullptr);


    /*!
     * \brief Destructor By default
     */
	~CLdapData();
public:

    /*!
     * @brief Method initializes internal data (once on run-time)
     */
    static void initialize();

    /*!
     * @brief Method To try to connect to server. onConnectionCompleted will fired with success ot failed status.
     * @param connectOptions The settings to use fot connction
     */
	void connect(const tConnectionOptions& connectOptions);

    /*!
     * @brief Method Current connection instance wiil be stopped and connect to server, based on saved tConnectionOptions instance.
     */
    void reconnect();

    /*!
     * @brief Method Clearing internal list of entries and then filling from server list of entries
     */
    void rebuild();

    /*!
     * @brief Method returns top list of entries
     * @return  QVector<CLdapEntry*> data
     */
	QVector<CLdapEntry*> topList();

    /*!
     * @brief Method Perform search action on server and get result
     * @param searchOptions The options to search
     * @return QStringList data
     */
	QStringList search(const tSearchOptions& searchOptions);

    /*!
     * @brief Method returns main instance of schema of ldap server (classes, attributes etc.)
     * @return CLdapSchema data
     */
	CLdapSchema& schema();

    /*!
     * @brief Method returns main instance of server
     * @return CLdapServer data
     */
    CLdapServer& server();

    /*!
     * @brief Method returns host of ldap server
     * @return QString data
     */
	QString host();

    /*!
     * @brief Method returns port of ldap server
     * @return int data
     */
	int port();

    /*!
     * @brief Method returns distinguished name of ldap server
     * @return int data
     */
	QString baseDN();

    /*!
     * @brief Method check given dn on exist on ldap server
     * @param dn distinguished name to check
     * @return bool true if exist, otherwise false
     */
    bool isDnExist(QString dn);

    /*!
     * \brief Method for changing user's passowrd by administrator
     * \param userDN DN of user for changing password
     * \param newPassword new password string

     Throws CLdapServerException with description if got error from server
     */
    void changeUserPassword(CLdapEntry* entry,const QString& userDN, const QString& newPassword);

Q_SIGNALS:
    void onConnectionCompleted(bool isSucceed, QString errorDescription);

private:

    /*!
     * @brief Method To build internal list of entries
     */
	void build();

    /*!
     * @brief Method To clear instance of connection
     */
    void resetConnection();

    /*!
     * @brief Method Using to reconnect to server, when before has broken TCP connection
     */
    bool hardReconnect();

private:


    /*!
     * @brief Method installs a Qt message handler which has been defined previously
     * @param type Qt context
     * @param context This enum describes the messages that can be sent to a message handler (QtMessageHandler)
     * @param msg the message
     */
    static void syslogMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

private:
    /*!
     * @brief member m_Entries list of top entries
     */
    QVector<CLdapEntry*>        m_Entries;

    /*!
     * @brief member m_searchOptions Saved search options to search action on server
     */
    tSearchOptions              m_searchOptions;

    /*!
     * @brief member m_connectOptions Saved connection options to connect action on server
     */
    tConnectionOptions          m_connectOptions;

    /*!
     * @brief member m_Connection Instance of connection
     */
    std::unique_ptr<LDAPConnection> m_Connection;

    /*!
     * @brief member m_Schema Instance of schema
     */
    CLdapSchema                 m_Schema;

    /*!
     * @brief member m_Schema Instance of server
     */
    CLdapServer                 m_Server;

    /*!
     * @brief member m_baseDN Distinguished name of ldap server
     */
    std::string                 m_baseDN;

    /*!
     * @brief member m_CanUseUntrustedConnection If can use untrusted connection is true, otherwise false
     */
    bool                        m_CanUseUntrustedConnection {false};

    friend class CLdapEntry;
};

}

#endif // CLdapData_H
