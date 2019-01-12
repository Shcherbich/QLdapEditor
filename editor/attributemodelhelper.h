#ifndef ATTRIBUTEMODELHELPER_H
#define ATTRIBUTEMODELHELPER_H

#include <QObject>
#include <QModelIndex>
#include "CLdapAttribute.h"

namespace ldapeditor
{
class CAttributeModelHelper
{
public:
    CAttributeModelHelper();

    QVariant data(const ldapcore::CLdapAttribute &attr, const QModelIndex &index, int role = Qt::DisplayRole)const;
    bool setData(ldapcore::CLdapAttribute&  attr, const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
    typedef struct _tAttrHelper
    {
        QString title;
    }tAttrHelper;

    typedef QMap<ldapcore::AttrType, tAttrHelper> tAttrHelperMap;
    tAttrHelperMap m_attrMap;

    QString attributeType2String(ldapcore::AttrType type) const;
    QString formatValueByType(const ldapcore::CLdapAttribute &attr)const;

    QString displayRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant editRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant tooltipRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;
    QVariant foregroundRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    bool setEditRoleData(ldapcore::CLdapAttribute &attr, const QVariant& value, const QModelIndex &index);
};
} //namespace ldapeditor
#endif // ATTRIBUTEMODELHELPER_H
