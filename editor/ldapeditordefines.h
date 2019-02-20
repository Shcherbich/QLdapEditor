/*!
@file
@brief Header file for defines used in project

File contains  defines used in project
*/

#ifndef LDAPEDITORDEFINES_H
#define LDAPEDITORDEFINES_H

#include <qnamespace.h>
#include "version.h" // use generated version file

/*!
  @brief Defines name of organization
*/
#define OrganizationName COMPANY

/*!
  @brief Defines name of application
*/
#define ApplicationName  PRODUCT

/*!
  @brief Defines version of application
*/
#define ApplicationVersion VERSION

namespace ldapeditor
{
    /*!
     * @ingroup ldapeditor
     * @brief The LDapEditorRoles enumumeraion for attributes model
     *
     * Set of values for custom roles IDs used in attributes model
     */
    enum LDapEditorRoles
    {
        AttributesListRole  = Qt::UserRole + 1, ///< List of attributes
        AttrNameRole,                           ///< Attribute name
        AttrValueRole,                          ///< Attribute Value
        AttrTypeRole,                           ///< Attribute type
        AttrSizeRole                            ///< Attribute Size
    } ;

    /*!
     * @ingroup ldapeditor
     * @brief The LdapTreeRoles enumumeraion for tree model
     *
     * Set of values for custom roles IDs used in tree model
     */
    enum LdapTreeRoles
    {
        TreeDnRole  = Qt::UserRole + 1,       ///< DN data
    } ;
}

#endif // LDAPEDITORDEFINES_H
