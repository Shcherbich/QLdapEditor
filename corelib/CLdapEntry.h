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
    explicit CLdapEntry(CLdapEntry* parentLdapEntry, LDAPEntry* le, QObject *parent = nullptr);
    ~CLdapEntry();

    CLdapEntry* parent();
    QVector<CLdapEntry*> children();
    QVector<CLdapAttribute> attributes();

public:
    QString dn();
    QString rDn();

signals:

public slots:

private:
    void construct(LDAPConnection* conn, QString baseDn);

private:
    CLdapEntry*           m_pParent;
    LDAPEntry*            m_pEntry;
    QVector<CLdapEntry*>  m_pEntries;
    QString               m_baseDn;
    QString               m_rDn;

    friend class CLdapData;
};

}

#endif // CLDAPENTRY_H
