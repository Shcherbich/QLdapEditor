#ifndef CLDAPATTRIBUTE_H
#define CLDAPATTRIBUTE_H

//#include <share.h>
#include <QObject>
#include <QString>

namespace ldapcore
{

enum  class AttrType
{
    Int,
    Binary,
    String,
    Date,
    Time
};



//class LDAPEDITORCORE_SO_API CLdapAttribute : public QObject
//class CLdapAttribute : public QObject
class CLdapAttribute
{
    //Q_OBJECT
public:
    //explicit CLdapAttribute(QObject *parent = nullptr);
    //explicit CLdapAttribute(QString name, QString value, AttrType type, QObject *parent = nullptr);

    explicit CLdapAttribute();
    explicit CLdapAttribute(QString name, QString value, AttrType type);

public:

    QString name() const;
    QString value() const;
    AttrType type() const;
    QString typeAsString()const;


private:
    QString m_Name;
    QString m_Value;
    AttrType m_Type;

    QString type2String(AttrType type) const;

signals:

public slots:
};

}

#endif // CLDAPATTRIBUTE_H
