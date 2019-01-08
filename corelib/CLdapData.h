#ifndef CLdapData_H
#define CLdapData_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include "CLdapObject.h"
#include "CLdapEntry.h"
#include <memory>

class LDAPConnection;

namespace ldapcore
{

typedef struct _tConnectionOptions
{
    std::string host;
    int port{389};
    int version{3};
    std::string basedn;
    std::string username;
    std::string password;
    bool simpleAuth{true};
    bool useSSL{false};
    bool useTLS{false};
    bool useSASL{false};
    bool useAnonymous{false};
}tConnectionOptions;

typedef struct _tSearchOptions
{
    int scope{1}; //LDAPConnection::SEARCH_ONE
     std::string basedn;
     std::string filter{"objectClass=*"};
     std::string  attributes;
}tSearchOptions;

//class LDAPEDITORCORE_SO_API CLdapData :  public QObject
class CLdapData :  public QObject
{
    Q_OBJECT

public:
    explicit CLdapData(QObject* parent = nullptr);
    ~CLdapData();
public:

    void Connect(const tConnectionOptions& connectOptions);
    void Disconnect();
    QVector<CLdapEntry*> TopList();
    void AddObject(QString id);
    void DeleteObject(CLdapObject*);

    QStringList Search(const tSearchOptions& searchOptions);

    QString host();
    int port();
    QString baseDN();

signals:
    void OnConnectionCompleted(CLdapData* pThis, bool isSucceed, QString errorDescription);
    void OnObjectAdded(CLdapObject* p, bool isSucceed, QString errorDescription);
    void OnObjectDeleted(CLdapObject* p, bool isSucceed, QString errorDescription);

public:

private:
    void Build();

private:
    QVector<CLdapEntry*> m_Entries;
    std::unique_ptr<LDAPConnection> m_Connection;
    std::string m_baseDN;
};

}

#endif // CLdapData_H
