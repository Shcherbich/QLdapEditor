#ifndef ATTRIBUTEMODELHELPER_H
#define ATTRIBUTEMODELHELPER_H

#include <QObject>
#include <QModelIndex>
#include "CLdapAttribute.h"
#include "CLdapData.h"

namespace ldapeditor
{
enum class AttributeColumn:int {
    Name = 0,
    Class,
    Attribute,
    Value,
    Type,
    Size
};

class CAttributeModelHelper
{
public:
    CAttributeModelHelper(ldapcore::CLdapData &ldapData);
    void setLdapEntry(ldapcore::CLdapEntry* entry);

    QVariant data(const ldapcore::CLdapAttribute &attr, const QModelIndex &index, int role = Qt::DisplayRole)const;
    bool setData(ldapcore::CLdapAttribute&  attr, const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    QString attributeType2String(ldapcore::AttrType type) const;

private:
    ldapcore::CLdapData& m_LdapData;
    ldapcore::CLdapEntry* m_LdapEntry{nullptr};

    typedef struct _tAttrHelper
    {
        QString title;
    }tAttrHelper;

    typedef QMap<ldapcore::AttrType, tAttrHelper> tAttrHelperMap;
    tAttrHelperMap m_attrMap;


    QString formatValueByType(const ldapcore::CLdapAttribute &attr)const;

    QString displayRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant editRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant tooltipRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant foregroundRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant fontRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    bool setEditRoleData(ldapcore::CLdapAttribute &attr, const QVariant& value, const QModelIndex &index);
    QString displayClassInfo(const ldapcore::CLdapAttribute &attr) const;
};
} //namespace ldapeditor
#endif // ATTRIBUTEMODELHELPER_H
