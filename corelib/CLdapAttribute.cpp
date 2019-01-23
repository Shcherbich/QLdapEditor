#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute()
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type, bool isMust, AttributeState editState)
    : m_Name(name), m_Value(value), m_Type(type), m_editState(editState), m_isMust(isMust)
{
	if (m_editState == AttributeState::AttributeReadWrite)
	{
		m_isModified = true;
	}
}

CLdapAttribute::CLdapAttribute(const CLdapAttribute& src)
{
	if (this != &src)
	{
		m_Name = src.m_Name;
		m_Value = src.m_Value;
		m_Type = src.m_Type;
		m_editState = src.m_editState;
		m_isModified = src.isModified();
        m_isMust = src.m_isMust;
	}
}

CLdapAttribute::CLdapAttribute(CLdapAttribute&& temp)
 : m_Name(std::move(temp.m_Name)), m_Value(std::move(temp.m_Value)),
   m_Type(temp.m_Type), m_editState(temp.m_editState), m_isMust(temp.m_isMust)
{

}

CLdapAttribute& CLdapAttribute::operator = (const CLdapAttribute& src)
{
    if (this == &src)
        return *this;
    m_Name = src.m_Name;
    m_Value = src.m_Value;
    m_Type = src.m_Type;
    m_editState = src.m_editState;
    m_isModified = src.isModified();
    m_isMust = src.m_isMust;
    return * this;
}

CLdapAttribute& CLdapAttribute::operator = (CLdapAttribute&& src)
{
    m_Name = std::move(src.m_Name);
    m_Value = std::move(src.m_Value);
    m_Type = src.m_Type;
    m_editState = src.m_editState;
    m_isModified = src.isModified();
    m_isMust = src.m_isMust;
    return * this;
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

bool CLdapAttribute::isMust() const
{
    return m_isMust;
}

void CLdapAttribute::setName(const QString& name)
{
	if (editState() == AttributeState::AttributeReadWrite)
	{
		m_Name = name;
		m_isModified = true;
	}
}
void CLdapAttribute::setValue(const QString& value)
{
	if (editState() != AttributeState::AttributeReadOnly)
	{
		if (!validateValue(value))
		{
			throw std::invalid_argument("can't set value due to wrong data format");
		}

		m_Value = value;
		m_isModified = true;
	}
}

void CLdapAttribute::setType(AttrType type)
{
	m_Type = type;
	m_isModified = true;
}

void CLdapAttribute::setEditState(AttributeState state)
{
    m_editState = state;
}

bool CLdapAttribute::validateValue(const QString& value)
{
	return true;
}

}

