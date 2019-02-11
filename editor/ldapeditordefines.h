#ifndef LDAPEDITORDEFINES_H
#define LDAPEDITORDEFINES_H

#include <qnamespace.h>
#include "version.h" // use generated version file

#define OrganizationName COMPANY
#define ApplicationName  PRODUCT
#define ApplicationVersion VERSION

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

    enum LdapTreeRoles
    {
        TreeDnRole  = Qt::UserRole + 1,
    } ;
}

//#define AttributesList  Qt::UserRole + 1
//#define AttrNameRole    Qt::UserRole + 2
//#define AttrValueRole   Qt::UserRole + 3
//#define AttrTypeRole    Qt::UserRole + 4
//#define AttrSizeRole    Qt::UserRole + 5

#endif // LDAPEDITORDEFINES_H
