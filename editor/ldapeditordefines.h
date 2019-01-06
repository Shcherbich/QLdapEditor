#ifndef LDAPEDITORDEFINES_H
#define LDAPEDITORDEFINES_H

#include <qnamespace.h>


#define OrganizationName "Very big company Inc."
#define ApplicationName  "LdapEditorApp"
#define ApplicationVersion "0.1"

namespace ldapeditor
{
    enum LDapEditorRoles
    {
        AttributesListRole  = Qt::UserRole + 1,
        AttrNameRole,
        AttrValueRole,
        AttrTypeRole,
        AttrSizeRole
    } ;
}

//#define AttributesList  Qt::UserRole + 1
//#define AttrNameRole    Qt::UserRole + 2
//#define AttrValueRole   Qt::UserRole + 3
//#define AttrTypeRole    Qt::UserRole + 4
//#define AttrSizeRole    Qt::UserRole + 5

#endif // LDAPEDITORDEFINES_H
