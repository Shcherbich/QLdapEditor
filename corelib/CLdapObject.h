#ifndef CLDAPOBJECT_H
#define CLDAPOBJECT_H

//#include <share.h>
#include <QObject>
#include <QString>
#include <QVector>
#include "CLdapAttribute.h"

namespace ldapcore
{
//class LDAPEDITORCORE_SO_API CLdapObject : public QObject
class CLdapObject : public QObject
{
    Q_OBJECT
public:
    explicit CLdapObject(QObject *parent = nullptr);
    explicit CLdapObject(QString id, QObject *parent = nullptr);


public:
    QString GetId();
    QVector<CLdapObject*> GetChildObjects();
    QVector<CLdapAttribute*> GetAttributes();
    void AddAttribute(QString name, QString value, AttrType type);
    void DeleteAttribute(CLdapAttribute*);

signals:

    void OnAttributeAdded(CLdapAttribute* pObject, bool isSucceed, QString errorDescription);
    void OnAttributeDeleted(CLdapAttribute* pObject, bool isSucceed, QString errorDescription);

public slots:

private:

    QString m_Id;
    QVector<CLdapObject*> m_Childs;
    QVector<CLdapAttribute*> m_Attributes;
};

}

#endif // CLDAPOBJECT_H

