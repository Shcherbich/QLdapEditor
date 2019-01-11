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

class CLdapData;

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
    void construct(CLdapData* data, LDAPConnection* conn, QString baseDn);

private:
    CLdapEntry*           m_pParent{nullptr};
    LDAPEntry*            m_pEntry{nullptr};
    QVector<CLdapEntry*>  m_pEntries;
    QString               m_baseDn;
    QString               m_rDn;
    CLdapData*            m_pData;

    friend class CLdapData;
};

}

#endif // CLDAPENTRY_H
