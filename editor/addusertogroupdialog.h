/*!
@file
@brief Header file for CAddUserToGroupDialog class

File contains  declarations for CAddUserToGroupDialog class
*/

#ifndef ADDUSERTOGROUPDIALOG_H
#define ADDUSERTOGROUPDIALOG_H

#include <CLdapData.h>
#include <QDialog>
#include "ldapsettings.h"

namespace Ui {
class CAddUserToGroupDialog;
}

namespace ldapeditor
{
    class CLdapSettings;

    /*!
     * @ingroup ldapeditor
     * @brief The CAddUserToGroupDialog class
     *
     * Provides functionality for add/remove user to group
     */
    class CAddUserToGroupDialog : public QDialog
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor CAddUserToGroupDialog
         * \param ldapData reference t oldapcore::CLdapData
         * \param settings reference to application's settings
         * \param parent pointer to parent QWidget
         */
        explicit CAddUserToGroupDialog(ldapcore::CLdapData&  ldapData, ldapeditor::CLdapSettings& settings, QWidget *parent = nullptr);

        /*!
           * \brief Destructor CAddUserToGroupDialog
        */
        ~CAddUserToGroupDialog();

        /*!
         * \brief Method for return array of users -  members of group
         * \return array of string with DN of users in group
         */
        QStringList membersList() const;

        /*!
         * \brief Method sets array of users -  members of group
         * \param list of string with DN of users in group
         */
        void setMembersList(const QStringList& list);
    protected slots:

        /*!
         * \brief Protected slot, called when user changed selection of items in 'all users list'
         */
        void onAllUsersSelectionChanged();

        /*!
         * \brief Protected slot, called when user wants to add item to group
         */
        void onAddUserToGroup();

        /*!
         * \brief Protected slot, called when user changed selection of items in 'group's users list'
         */
        void onGroupUsersSelectionChanged();

        /*!
         * \brief Protected slot, called when user wants to remove item from group
         */
        void onRemoveUserFromGroup();

    private:
        Ui::CAddUserToGroupDialog *ui; ///< pointer yo UI implementation memeber
    };
} //namespace ldapeditor

#endif // ADDUSERTOGROUPDIALOG_H
