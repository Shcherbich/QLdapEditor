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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)const;

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
};
} //namespace ldapeditor
#endif // ATTRIBUTEMODELHELPER_H
