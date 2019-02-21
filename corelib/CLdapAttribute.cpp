#include "CLdapAttribute.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapAttribute::CLdapAttribute()
{

}

CLdapAttribute::CLdapAttribute(QString name, QString value, AttrType type, bool isMust, QString desc, QStringList& classes, AttributeState editState)
    : m_Name(name), m_Value(value), m_Type(type), m_isMust(isMust), m_Description(desc), m_Classes(classes), m_editState(editState)
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
        m_Description = src.m_Description;
        m_Classes << src.m_Classes;
        m_isIgnore = src.m_isIgnore;
	}
}

CLdapAttribute::CLdapAttribute(CLdapAttribute&& temp)
 : m_Name(std::move(temp.m_Name)),
   m_Value(std::move(temp.m_Value)),
   m_Type(temp.m_Type),
   m_isMust(temp.m_isMust),
   m_Description(std::move(temp.m_Description)),
   m_Classes(std::move(temp.m_Classes)),
   m_editState(temp.m_editState),
   m_isIgnore(std::move(temp.m_isIgnore))
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
    m_Description = src.m_Description;
    m_Classes << src.m_Classes;
    m_isIgnore = src.m_isIgnore;
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
    m_Description = src.m_Description;
    m_Classes = std::move(src.m_Classes);
    m_isIgnore = src.m_isIgnore;
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

bool CLdapAttribute::validateValue(const QString&)
{
    return true;
}

QString CLdapAttribute::description() const
{
    return m_Description;
}

void CLdapAttribute::setDescription(QString& desc)
{
    m_Description = desc;
    m_isModified = true;
}

QStringList CLdapAttribute::classes() const
{
    return m_Classes;
}

void CLdapAttribute::setClasses(QStringList& v)
{
    m_Classes.clear();
    m_Classes << v;
    m_isModified = true;
}


}

