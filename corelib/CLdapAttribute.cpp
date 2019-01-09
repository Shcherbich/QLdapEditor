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

QString CLdapAttribute::typeAsString()const
{
    return type2String(m_Type);
}

QString CLdapAttribute::type2String(AttrType type) const
{
    switch(type)
    {
    case AttrType::Int: return QString("Int");
    case AttrType::Binary: return QString("Binary");
    case AttrType::String: return QString("String");
    case AttrType::Date: return QString("Date");
    case AttrType::Time: return QString("Time");
    }
    return QString("???");
}
}

