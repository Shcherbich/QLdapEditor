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
/*
void CLdapObject::AddAttribute(QString name, QString value, AttrType type)
{
    CLdapAttribute* pAttr = new CLdapAttribute(name, value, type);
    QThreadPool::globalInstance()->start(makeSimpleTask([=]
    {
        std::this_thread::sleep_for (std::chrono::seconds(2));
        m_Attributes << pAttr;
        emit OnAttributeAdded(pAttr, true, QString(""));
    }));
}

void CLdapObject::DeleteAttribute(CLdapAttribute* pDelete)
{
    auto f = std::find_if(m_Attributes.begin(), m_Attributes.end(), [=](const CLdapAttribute* p) { return p == pDelete; });
    if (f == m_Attributes.end())
    {
        emit OnAttributeDeleted(pDelete, false, QString("No found!"));
    }
    else
    {
        QThreadPool::globalInstance()->start(makeSimpleTask([=]
        {
            std::this_thread::sleep_for (std::chrono::seconds(1));
            m_Attributes.erase(f);
            emit OnAttributeDeleted(pDelete, true, QString(""));

        }));
    }
}
*/
}
