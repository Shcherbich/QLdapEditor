#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute()
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type, AttributeState editState)
    :m_Name(name), m_Value(value), m_Type(type), m_editState(editState)
{
    if(m_editState == AttributeState::AttributeReadWrite)
        m_isModified = true;
}

CLdapAttribute::CLdapAttribute(const CLdapAttribute& src)
{
    if( this != &src)
    {
        m_Name = src.m_Name;
        m_Value = src.m_Value;
        m_Type = src.m_Type;
        m_editState = src.m_editState;
        m_isModified = src.isModified();
    }
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

AttributeState CLdapAttribute::editState()const
{
    return m_editState;
}

bool CLdapAttribute::isModified() const
{
    return m_isModified;
}

void CLdapAttribute::setName(const QString& name)
{
    if(editState() == AttributeState::AttributeReadWrite)
    {
        m_Name = name;
        m_isModified = true;
    }
}
void CLdapAttribute::setValue(const QString& value)
{
    if(editState() != AttributeState::AttributeReadOnly)
    {
        if(!validateValue(value))
            throw std::invalid_argument("can't set value due to wrong data format");

         m_Value = value;
         m_isModified = true;
    }
}

void CLdapAttribute::setType(AttrType type)
{
    m_Type = type;
    m_isModified = true;
}

bool CLdapAttribute::validateValue(const QString& value)
{
    return true;
}

}

