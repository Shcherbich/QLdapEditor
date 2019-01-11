#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute()
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type, bool editable)
    :m_Name(name), m_Value(value), m_Type(type), m_Editable(editable)
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

bool CLdapAttribute::editable()const
{
    return m_Editable;
}

}

