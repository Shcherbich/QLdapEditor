#include <string>
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <memory>
#include <QDebug>
#include "cldapserver.h"
#include "LDAPEntry.h"
#include "LDAPException.h"
#include "LDAPConnection.h"
#include "LDAPModification.h"
#include "LDAPSchema.h"
#include "LDAPMessage.h"

using uEntry = std::shared_ptr<LDAPEntry>;

namespace ldapcore
{

CLdapServer::CLdapServer()
{

}

void CLdapServer::add(CLdapEntry& entry) noexcept(false)
{
    bool isUser = false;
    QStringList log;
    try
    {
        std::shared_ptr<LDAPAttributeList> attrs(new LDAPAttributeList());
        StringList objectClasses;
        for (auto& c : entry.m_classes)
        {
            if (c != "top")
            {
                objectClasses.add(c.toStdString());
                QString l = QString("Adding attribute %1, value %2. ").arg("objectClass").arg(c);
                log.push_back(l);
            }
            isUser = isUser ? isUser : c == "user";
        }
        attrs->addAttribute(LDAPAttribute("objectClass", objectClasses));
        static std::set<QString> excludedAttributeNames {"cn"};

        for (auto& a : entry.m_attributes)
        {
            auto value = a.value();
            if (!value.isEmpty() && a.name() != "objectClass")
            {
                attrs->addAttribute(LDAPAttribute(a.name().toStdString(), value.toStdString()));
                QString l = QString("Adding attribute %1, value %2. ").arg(a.name()).arg(value);
                log.push_back(l);
            }
        }
        std::shared_ptr<LDAPEntry> e(new LDAPEntry(entry.m_pEntry->getDN(), attrs.get()));
        auto dn = e->getDN();
        entry.connectionPtr()->add(e.get());
        entry.m_isNew = false;
        entry.flushAttributesCache();
        if (isUser)
        {
            auto cn = entry.m_pEntry->getAttributeByName("cn");
            auto sAMAccountName = entry.m_pEntry->getAttributeByName("sAMAccountName");
            if (cn != nullptr && sAMAccountName != nullptr) // to sambe needs to set 'sAMAccountName' as 'cn' value
            {
                try
                {
                    std::unique_ptr<LDAPModList> mod(new LDAPModList());
                    mod->addModification(LDAPModification(LDAPAttribute("sAMAccountName", cn->toString()), LDAPModification::OP_REPLACE));
                    entry.connectionPtr()->modify_s(dn, mod.get());
                    entry.flushAttributesCache();
                }
                catch (const std::exception& e)
                {
                    Q_UNUSED(e);
                }
            }
        }

        {
            qWarning() << QString("Successfully added entry %1. %2").arg(entry.dn()).arg(log.join(";"));
        }
    }
    catch (const std::exception& ex)
    {
        {
            qCritical() << QString("Failed to add entry %1. %2. %3").arg(entry.dn()).arg(ex.what()).arg(log.join(";"));
        }
        throw CLdapServerException(ex.what());
    }
}

void CLdapServer::update(CLdapEntry& entry) noexcept(false)
{
    if (entry.m_isEdit == false)
    {
        return;
    }
    QStringList log;
    try
    {
        QVector<CLdapAttribute> realAttributes;
        entry.loadAttributes(realAttributes);

        std::unique_ptr<LDAPModList> mod(new LDAPModList());

        // 1. Add/Modify attributes
        StringList objectClasses;
        for (auto& c : entry.m_classes)
        {
            objectClasses.add(c.toStdString());
            QString l = QString("Adding attribute %1, value %2. ").arg("objectClass").arg(c);
            log.push_back(l);

        }
        mod->addModification(LDAPModification(LDAPAttribute("objectClass", objectClasses), LDAPModification::OP_REPLACE));
        for (auto& a : entry.m_attributes)
        {

            if (a.name() == "objectClass")
            {
                continue;
            }

            auto f = std::find_if(realAttributes.begin(), realAttributes.end(),
                                  [&](const ldapcore::CLdapAttribute & o)
            {
                return a.name().compare(o.name(), Qt::CaseInsensitive) == 0 && a.value() == o.value();
            });

            // no modification
            if (f != realAttributes.end())
            {
                continue;
            }

            auto value = a.value().toStdString();

            // add modification
            if (!value.empty())
            {
                if (f == realAttributes.end())
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_ADD));

                    QString l = QString("Adding attribute %1, value %2").arg(a.name()).arg(value.c_str());
                    log.push_back(l);
                }
                else
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_REPLACE));
                    QString l = QString("Updating attribute %1, value %2").arg(a.name()).arg(value.c_str());
                    log.push_back(l);
                }
            }
        }

        // 2. Delete attributes
        std::set<QString> setOfAttributes;
        for (auto& a : entry.m_attributes)
        {
            setOfAttributes.insert(a.name());
        }
        for (auto& a : realAttributes)
        {
            if (setOfAttributes.find(a.name()) != setOfAttributes.end())
            {
                continue;
            }
            mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), a.value().toStdString()),
                                                  LDAPModification::OP_DELETE));
            QString l = QString("Deleting attribute %1, value %2").arg(a.name()).arg(a.value());
            log.push_back(l);
        }

        auto& dn = entry.m_pEntry->getDN();
        entry.connectionPtr()->modify_s(dn, mod.get());
        entry.m_isEdit = false;

        {
            qWarning() << QString("Successfully updated entry %1/%2. %3").arg(entry.dn()).arg(entry.guid()).arg(log.join(";"));
        }
    }
    catch (const std::exception& ex)
    {
        {
            qCritical() << QString("Failed to update entry %1/%2. %3. %4").arg(entry.dn()).arg(entry.guid()).arg(ex.what()).arg(log.join(";"));
        }

        throw CLdapServerException(ex.what());
    }
}

void CLdapServer::del(CLdapEntry& entry) noexcept(false)
{
    try
    {
        std::unique_ptr<LDAPConstraints> cons = std::unique_ptr<LDAPConstraints>(new LDAPConstraints());
        auto& dn = entry.m_pEntry->getDN();
        entry.connectionPtr()->del(dn, cons.get());
        {
            qWarning() << QString("Successfully deleted entry %1/%2.").arg(entry.dn()).arg(entry.guid());
        }

    }
    catch (const std::exception& ex)
    {
        {
            qCritical() << QString("Failed to delete entry %1/%2. %3").arg(entry.dn()).arg(entry.guid()).arg(ex.what());
        }

        throw CLdapServerException(ex.what());
    }
}

void CLdapServer::addAttribute(CLdapEntry& entry, CLdapAttribute& attribute) noexcept(false)
{
    try
    {
        LDAPAttribute newattr(attribute.name().toStdString());
        newattr.addValue(attribute.value().toStdString());
        LDAPModification::mod_op op = LDAPModification::OP_ADD;
        std::unique_ptr<LDAPModList>  mod(new LDAPModList());
        mod->addModification(LDAPModification(newattr, op));
        entry.connectionPtr()->modify_s(entry.m_pEntry->getDN(), mod.get());

        {
            QString log = QString("Successfully added attribute %1 %3/%4, value %2").
                    arg(attribute.name()).arg(attribute.value()).arg(entry.dn()).arg(entry.guid());
            qWarning() << log;
        }

    }
    catch (const std::exception& ex)
    {
        {
            QString log = QString("Failed to add attribute %1 of %4/%5, value %2. %3").
                    arg(attribute.name()).arg(attribute.value()).arg(ex.what()).arg(entry.dn()).arg(entry.guid());
            qCritical() << log;
        }


        auto err = QString("Add new attribute %1: %2").arg(attribute.name()).arg(ex.what());
        throw CLdapServerException(err.toStdString().c_str());
    }

}

void CLdapServer::updateAttributes(CLdapEntry& entry, QString name, const QVector<ldapcore::CLdapAttribute>& values) noexcept(false)
{
    QStringList log;
    try
    {
        LDAPModification::mod_op op = LDAPModification::OP_REPLACE;
        std::unique_ptr<LDAPModList> mod(new LDAPModList());
        auto q = entry.connectionPtr()->search(entry.m_pEntry->getDN(), LDAPAsynConnection::SEARCH_SUB, "objectClass=*", StringList());
        uEntry en(q->getNext());
        auto find = en->getAttributeByName(name.toStdString());
        if (find == nullptr)
        {
            throw ldapcore::CLdapServerException("No found attribute");
        }
        StringList newList;
        for (auto& v : values)
            if (!v.value().isEmpty())
            {
                newList.add(v.value().toStdString());
                log.push_back(v.value());
            }
        const_cast<LDAPAttribute*>(find)->setValues(newList);
        mod->addModification(LDAPModification(*find, op));
        entry.connectionPtr()->modify_s(entry.m_pEntry->getDN(), mod.get());

        {
            QString l = QString("Successfully updated attribute %1 %3/%4, value %2").
                    arg(name).arg(log.join(";")).arg(entry.dn()).arg(entry.guid());
            qWarning() << l;
        }

    }
    catch (const std::exception& ex)
    {
        {
            QString l = QString("Failed to update attribute %1 of %4/%5, value %2. %3").
                    arg(name).arg(log.join(";")).arg(ex.what()).arg(entry.dn()).arg(entry.guid());
            qCritical() << l;
        }

        auto err = QString("Update attribute %1: %2").arg(name).arg(ex.what());
        throw CLdapServerException(err.toStdString().c_str());
    }
}

void CLdapServer::delAttribute(CLdapEntry& entry, CLdapAttribute& attribute) noexcept(false)
{
    try
    {
        LDAPAttribute newattr(attribute.name().toStdString());
        LDAPModification::mod_op op = LDAPModification::OP_DELETE;
        std::unique_ptr<LDAPModList> mod(new LDAPModList());
        mod->addModification(LDAPModification(newattr, op));
        entry.connectionPtr()->modify_s(entry.m_pEntry->getDN(), mod.get());

        {
            QString l = QString("Successfully deleted attribute %1 %3/%4, value %2").
                    arg(attribute.name()).arg(attribute.value()).arg(entry.dn()).arg(entry.guid());
            qWarning() << l;
        }

    }
    catch (const std::exception& ex)
    {
        {
            QString l = QString("Failed to delete attribute %1 of %4/%5, value %2. %3").
                    arg(attribute.name()).arg(attribute.value()).arg(ex.what()).arg(entry.dn()).arg(entry.guid());
            qCritical() << l;
        }

        auto err = QString("Delete attribute %1: %2").arg(attribute.name()).arg(ex.what());
        throw CLdapServerException(err.toStdString().c_str());
    }
}

/*
 Based on some observing found, that 'userAccountControl' is status control flag of Samba AD user
 And second bit is enable flag of user
 if this flag = 0, user is active
 if this flag = 1, user is inactive and disable
 */
void CLdapServer::enableUser(CLdapEntry& entry) noexcept(false)
{
    auto userAccountControl = entry.m_pEntry->getAttributeByName("userAccountControl");
    if (userAccountControl == nullptr)
    {
        return;
    }
    try
    {
        bool ok;
        long val = QString(userAccountControl->toString().c_str()).toLong(&ok, 10);
        val &= ~2; // ENABLE user
        std::unique_ptr<LDAPModList> mod(new LDAPModList());
        mod->addModification(LDAPModification(LDAPAttribute("userAccountControl", QString("%1").arg(val).toStdString()),
                                              LDAPModification::OP_REPLACE));
        auto& dn = entry.m_pEntry->getDN();
        entry.connectionPtr()->modify_s(dn, mod.get());
        StringList values;
        values.add(QString("%1").arg(val).toStdString());
        const_cast<LDAPAttribute*>(userAccountControl)->setValues(values);
    }
    catch (const std::exception& e)
    {
        throw CLdapServerException(e.what());
    }
}

/*
 Based on some observing found, that 'userAccountControl' is status control flag of Samba AD user
 And second bit is enable flag of user
 if this flag = 0, user is active
 if this flag = 1, user is inactive and disable
 */
void CLdapServer::disableUser(CLdapEntry& entry) noexcept(false)
{
    auto userAccountControl = entry.m_pEntry->getAttributeByName("userAccountControl");
    if (userAccountControl == nullptr)
    {
        return;
    }
    try
    {
        bool ok;
        long val = QString(userAccountControl->toString().c_str()).toLong(&ok, 10);
        val |= 2; // DISABLE user
        std::unique_ptr<LDAPModList> mod(new LDAPModList());
        mod->addModification(LDAPModification(LDAPAttribute("userAccountControl", QString("%1").arg(val).toStdString()),
                                              LDAPModification::OP_REPLACE));
        auto& dn = entry.m_pEntry->getDN();
        entry.connectionPtr()->modify_s(dn, mod.get());
        StringList values;
        values.add(QString("%1").arg(val).toStdString());
        const_cast<LDAPAttribute*>(userAccountControl)->setValues(values);
    }
    catch (const std::exception& e)
    {
        throw CLdapServerException(e.what());
    }
}


}
