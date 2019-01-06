#ifndef CLDAPATTRIBUTE_H
#define CLDAPATTRIBUTE_H

//#include <share.h>
#include <QObject>
#include <QString>

namespace ldapcore
{

enum AttrType
{
    Int,
    Binary,
    String,
    Date,
    Time
};

//class LDAPEDITORCORE_SO_API CLdapAttribute : public QObject
class CLdapAttribute : public QObject
{
    Q_OBJECT
public:
    explicit CLdapAttribute(QObject *parent = nullptr);
    explicit CLdapAttribute(QString name, QString value, AttrType type, QObject *parent = nullptr);

public:

    QString GetName();
    QString GetValue();
    AttrType GetType();


private:
    QString m_Name;
    QString m_Value;
    AttrType m_Type;

signals:

public slots:
};

}

#endif // CLDAPATTRIBUTE_H
