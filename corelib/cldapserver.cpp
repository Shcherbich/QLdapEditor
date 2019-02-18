#include <string>
#include <vector>
#include <set>
#include <tuple>
#include <algorithm>
#include <memory>
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
    try
    {
        std::shared_ptr<LDAPAttributeList> attrs(new LDAPAttributeList());
        StringList objectClasses;
        for (auto& c : entry.m_classes)
        {
            if (c != "top")
            {
                objectClasses.add(c.toStdString());
            }
        }
        attrs->addAttribute(LDAPAttribute("objectClass", objectClasses));
        static std::set<QString> excludedAttributeNames {"cn"};

        for (auto& a : entry.m_attributes)
        {
            auto value = a.value();
            if (!value.isEmpty() && a.name() != "objectClass")
            {
                attrs->addAttribute(LDAPAttribute(a.name().toStdString(), value.toStdString()));
            }
        }
        std::shared_ptr<LDAPEntry> e(new LDAPEntry(entry.m_pEntry->getDN(), attrs.get()));
        auto dn = e->getDN();
        entry.connectionPtr()->add(e.get());
        entry.m_isNew = false;
        entry.flushAttributeCache();
    }
    catch (const std::exception& ex)
    {
        throw CLdapServerException(ex.what());
    }
}

void CLdapServer::update(CLdapEntry& entry) noexcept(false)
{
    if (entry.m_isEdit == false)
    {
        return;
    }
    try
    {
        QVector<CLdapAttribute> realAttributes;
        entry.loadAttributes(realAttributes);

        LDAPModList* mod = new LDAPModList();

        // 1. Add/Modify attributes
        StringList objectClasses;
        for (auto& c : entry.m_classes)
        {
            objectClasses.add(c.toStdString());
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
                return strcasecmp(o.name().toStdString().c_str(), a.name().toStdString().c_str()) == 0;
            });

            auto value = a.value().toStdString();

            // no modification
            if (f != realAttributes.end() && f->value() == a.value())
            {
                continue;
            }

            // add modification
            if (!value.empty())
            {
                if (f == realAttributes.end())
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_ADD));

                }
                else
                {
                    mod->addModification(LDAPModification(LDAPAttribute(a.name().toStdString(), value),
                                                          LDAPModification::OP_REPLACE));

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
        }

        auto& dn = entry.m_pEntry->getDN();
        entry.connectionPtr()->modify_s(dn, mod);
        entry.m_isEdit = false;
    }
    catch (const std::exception& ex)
    {
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
    }
    catch (const std::exception& ex)
    {
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
    }
    catch (const std::exception& ex)
    {
        auto err = QString("Add new attribute '%1': %2").arg(attribute.name()).arg(ex.what());
        throw CLdapServerException(err.toStdString().c_str());
    }

}

void CLdapServer::updateAttribute(CLdapEntry& entry, CLdapAttribute& attribute) noexcept(false)
{
    try
    {
        LDAPModification::mod_op op = LDAPModification::OP_REPLACE;
        LDAPModList* mod = new LDAPModList();
        auto q = entry.connectionPtr()->search(entry.m_pEntry->getDN(), LDAPAsynConnection::SEARCH_SUB, "objectClass=*", StringList());
        uEntry en(q->getNext());
        auto find = en->getAttributeByName(attribute.name().toStdString());
        if (find == nullptr)
        {
            throw ldapcore::CLdapServerException("No found attribute");
        }
        StringList newList;
        newList.add(attribute.value().toStdString());
        const_cast<LDAPAttribute*>(find)->setValues(newList);
        mod->addModification(LDAPModification(*find, op));
        entry.connectionPtr()->modify_s(entry.m_pEntry->getDN(), mod);
    }
    catch (const std::exception& ex)
    {
        auto err = QString("Update attribute '%1': %2").arg(attribute.name()).arg(ex.what());
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
    }
    catch (const std::exception& ex)
    {
        auto err = QString("Delete attribute '%1': %2").arg(attribute.name()).arg(ex.what());
        throw CLdapServerException(err.toStdString().c_str());
    }
}


}
