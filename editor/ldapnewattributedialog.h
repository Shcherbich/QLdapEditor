/*!
\file
\brief Header file for dialog 'Add Attribute' class

File contains  declarations for dialog 'Add Attribute' class
*/
#ifndef LDAPNEWATTRIBUTEDIALOG_H
#define LDAPNEWATTRIBUTEDIALOG_H

#include <QDialog>
#include "CLdapEntry.h"
#include "CLdapData.h"
#include "CLdapAttribute.h"
#include <QVector>
#include <QSet>

namespace Ui {
class CLdapNewAttributeDialog;
}

namespace ldapeditor
{

/*!
 * @ingroup ldapeditor
 * @brief The CLdapNewAttributeDialog class
 *
 * The CLdapNewAttributeDialog, used for adding new attribute to LDAP entity
 */
class CLdapNewAttributeDialog : public QDialog
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor CLdapNewAttributeDialog
     * \param ldapData refernce to ldapcore::CLdapData
     * \param entry pointer to LDAP entity
     * \param parent pointer to parent QObject
     */
    explicit CLdapNewAttributeDialog(ldapcore::CLdapData&  ldapData, ldapcore::CLdapEntry* entry, QWidget *parent = nullptr);

    /*!
    * \brief Destructor of CLdapNewAttributeDialog
    */
    ~CLdapNewAttributeDialog();

    /*!
     * \brief Method returns new created attribute
     * \return ldapcore::CLdapAttribute
     */
    ldapcore::CLdapAttribute attribute();
protected slots:
    /*!
     * \brief Protected slod, called when user changed class name
     * \param index index of class name
     */
    void onCurrentClassChanged(int index);

    /*!
     * \brief Protected slot, called when user changed attribute name
     * \param index index of attribute name
     */
    void onCurrentAttributeChanged(int index);
private:

    ldapcore::CLdapData&  m_LdapData;              ///< CLdapData reference member
    ldapcore::CLdapEntry* m_entry{nullptr};        ///< pointer to CLDapEntity memeber
    Ui::CLdapNewAttributeDialog *ui;               ///< pointer to UI implementation memeber
    QVector<ldapcore::CLdapAttribute> m_attributes;///< array of available attributes
    QVector<ldapcore::CLdapAttribute>* m_currentAttributes;///< array of current attributes

    QSet<QString> m_addMust; ///< set of unique Must attributes
    QSet<QString> m_addMay;  ///< set of unique May attributes
};
} //namespace ldapeditor
#endif // LDAPNEWATTRIBUTEDIALOG_H
