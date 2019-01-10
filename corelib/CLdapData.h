#ifndef CLdapData_H
#define CLdapData_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QList>
#include "CLdapObject.h"
#include "CLdapEntry.h"
#include "datatypes.h"
#include <memory>

class LDAPConnection;

namespace ldapcore
{
//class LDAPEDITORCORE_SO_API CLdapData :  public QObject
class CLdapData :  public QObject
{
    Q_OBJECT

public:
    explicit CLdapData(QObject* parent = nullptr);
    ~CLdapData();
public:

    void connect(const tConnectionOptions& connectOptions);
    void disconnect();
    QVector<CLdapEntry*> topList();
    void addObject(QString id);
    void deleteObject(CLdapObject*);

    QStringList search(const tSearchOptions& searchOptions);

    QString host();
    int port();
    QString baseDN();

signals:
    void OnConnectionCompleted(CLdapData* pThis, bool isSucceed, QString errorDescription);
    void OnObjectAdded(CLdapObject* p, bool isSucceed, QString errorDescription);
    void OnObjectDeleted(CLdapObject* p, bool isSucceed, QString errorDescription);

public:

private:
    void build();

private:
    QVector<CLdapEntry*> m_Entries;
    std::unique_ptr<LDAPConnection> m_Connection;
    std::string m_baseDN;
};

}

#endif // CLdapData_H
