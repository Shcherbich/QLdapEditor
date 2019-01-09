#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute()
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type)
    : m_Name(name), m_Value(value), m_Type(type)
{

}


QString CLdapAttribute::name()const
{
    return m_Name;
}

QString CLdapAttribute::value()const
{
    return m_Value;
}

AttrType CLdapAttribute::type()const
{
    return m_Type;
}


}

