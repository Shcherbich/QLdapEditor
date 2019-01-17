#include "CLdapObject.h"
#include "qfunctional.h"

namespace ldapcore
{


CLdapObject::CLdapObject(QObject *parent)
    : QObject(parent)
{

}

CLdapObject::CLdapObject(QString id, QObject *parent)
    : m_Id(id), QObject(parent)
{

}


QString CLdapObject::GetId()
{
    return m_Id;
}

QVector<CLdapObject*> CLdapObject::GetChildObjects()
{
    if (m_Childs.size())
    {
        return m_Childs;
    }
    QVector<CLdapObject*> vector;
    vector << new CLdapObject(QString("%1_%2").arg(m_Id, "Child1"), this);
    vector << new CLdapObject(QString("%1_%2").arg(m_Id, "Child99"), this);
    m_Childs.append(vector);
    return vector;
}

QVector<CLdapAttribute*> CLdapObject::GetAttributes()
{
    return m_Attributes;
}

}
