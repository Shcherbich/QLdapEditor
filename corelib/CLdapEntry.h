#ifndef CLDAPENTRY_H
#define CLDAPENTRY_H

#include <QObject>
#include <QString>
#include <QVector>
#include <memory>
#include "CLdapAttribute.h"

class LDAPEntry;
class LDAPConnection;

namespace ldapcore
{

class CLdapEntry : public QObject
{
    Q_OBJECT
public:
    explicit CLdapEntry(QObject *parent = nullptr);
    explicit CLdapEntry(LDAPEntry* le, QObject *parent = nullptr);
    ~CLdapEntry();

    void Construct(LDAPConnection* conn);
    QVector<CLdapEntry*> Children();
    QVector<CLdapAttribute> Attributes();

public:
    QString GetDn();

signals:

public slots:

private:
    LDAPEntry*           m_pEntry;
    QVector<CLdapEntry*>  m_pEntries;
};

}

#endif // CLDAPENTRY_H
