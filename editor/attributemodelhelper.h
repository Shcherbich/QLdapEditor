/*!
\file
\brief Header file for attribute helper class

File contains  declarations for attribute helper class
*/

#ifndef ATTRIBUTEMODELHELPER_H
#define ATTRIBUTEMODELHELPER_H

#include <QObject>
#include <QModelIndex>
#include "CLdapAttribute.h"
#include "CLdapData.h"

namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief The AttributeColumn enumumeraion
 *
 * Set of values Attribute column IDs for using instead of raw indexes
 */
enum class AttributeColumn:int {
    Name = 0,   ///< Attribute's DN column
    Class,      ///< Attribute's Class column
    Attribute,  ///< Attribute's Title column
    Value,      ///< Attribute's Value column
    Type,       ///< Attribute's Type column
    Size        ///< Attribute's Size column
};

/*!
 * @ingroup ldapeditor
 * @brief Attribute helper class
 *
 * Attribute helper class provides some common functionality for attribute's model
 *
 */
class CAttributeModelHelper
{
public:
    /*!
     * \brief Constructor CAttributeModelHelper
     * \param ldapData ldapcore::CLDapData reference object
     */
    CAttributeModelHelper(ldapcore::CLdapData &ldapData);

    /*!
     * @brief Method sets ldapcore::CLdapEntry pointer
     * @param entry pointer to ldapcore::CLdapEntry
     */
    void setLdapEntry(ldapcore::CLdapEntry* entry);

    /*!
     * @brief Method returns data from model
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for reading data
     * @param role required data role
     * @return  QVariant data
     */
    QVariant data(const ldapcore::CLdapAttribute &attr, const QModelIndex &index, int role = Qt::DisplayRole)const;

    /*!
     * @brief Method saves data into model after edit
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for reading data
     * @param value new value to be set
     * @param role equired data role
     * @return true if successful, false if error
     */
    bool setData(ldapcore::CLdapAttribute&  attr, const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /*!
     * @brief Method returns String representation of attribute type
     * @param type  attribute type
     * @return string representation
     */
    QString attributeType2String(ldapcore::AttrType type) const;

private:
    /*!
     * @brief member ldapcore::CLDapData reference object
     */
    ldapcore::CLdapData& m_LdapData;

    /*!
    * @brief member ldapcore::CLDapEntry poiner
    */
    ldapcore::CLdapEntry* m_LdapEntry{nullptr};

    /*!
     * @brief auxiliary structure for mapping Attribute types to String
    */
    typedef struct _tAttrHelper
    {
        QString title; ///< Title string
    }tAttrHelper;

    /*!
     * @typedef Map attribute type into string
     */
    typedef QMap<ldapcore::AttrType, tAttrHelper> tAttrHelperMap;

    /*!
     * @brief member Mapping attribute type into string
     */
    tAttrHelperMap m_attrMap;

    /*!
     * @brief Method returns formatted value according Attribute type
     * @param attr reference to ldapcore::CLdapAttribute
     * @return String representation of Attribute value
     */
    QString formatValueByType(const ldapcore::CLdapAttribute &attr)const;

    /*!
     * @brief Method returns display value
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for getting data
     * @return string representation of value
     */
    QString  displayRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    /*!
     * @brief Method returns value for editor
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for getting data
     * @return QVariant of data for editor
     */
    QVariant editRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    /*!
     * @brief Method returns tooltip string
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for getting data
     * @return QVariant of data for tooltip
     */
    QVariant tooltipRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    /*!
     * @brief Method returns foreground (text) color
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for getting data
     * @return QVariant of data for foreground (text) color
     */
    QVariant foregroundRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    /*!
     * @brief Method returns font
     * @param attr reference to ldapcore::CLdapAttribute
     * @param index index of item for getting data
     * @return QVariant of data for font
     */
    QVariant fontRoleData(const ldapcore::CLdapAttribute &attr, const QModelIndex &index)const;

    /*!
     * @brief Method for set data into model after edit
     * @param attr reference to ldapcore::CLdapAttribute
     * @param value new value for saving into model
     * @param index index of item for getting data
     * @return true if success, false if error
     */
    bool setEditRoleData(ldapcore::CLdapAttribute &attr, const QVariant& value, const QModelIndex &index);

    /*!
     * @brief Method returns string with Attribute Class description
     * @param attr reference to ldapcore::CLdapAttribute
     * @return string with attribute class description
     */
    QString displayClassInfo(const ldapcore::CLdapAttribute &attr) const;
};
} //namespace ldapeditor
#endif // ATTRIBUTEMODELHELPER_H
