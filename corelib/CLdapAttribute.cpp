#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute(QObject *parent) : QObject(parent)
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type, QObject *parent)
    : m_Name(name), m_Value(value), m_Type(type),  QObject(parent)
{

}

QString CLdapAttribute::GetName()
{
    return m_Name;
}

QString CLdapAttribute::GetValue()
{
    return m_Value;
}

AttrType CLdapAttribute::GetType()
{
    return m_Type;
}


}

