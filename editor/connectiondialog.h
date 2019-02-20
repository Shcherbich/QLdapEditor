/*!
\file
\brief Header file for LDAP Connection settings class

File contains  declarations for LDAP Connection settings class
*/
#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
class QSettings;

namespace Ui {
class CConnectionDialog;
}

namespace ldapcore {
class CLdapData;
}

namespace ldapeditor
{
    class CLdapSettings;

    /*!
     * @ingroup ldapeditor
     * @brief The CConnectionDialog class
     *
     * The CConnectionDialog class
     */
    class CConnectionDialog : public QDialog
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor CConnectionDialog
         * \param settings reference to application settings
         * \param ldapData reference to dapcore::CLdapData
         * \param parent pointer to parent QObject
         */
        explicit CConnectionDialog(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget *parent = nullptr);

        /*!
        * \brief Destructor CConnectionDialog
        */
        ~CConnectionDialog();
    private slots:
        /*!
         * \brief Provate slot, called when type of authentification is changed
         */
        void onAuthTypeChanged();

        /*!
         * \brief Private slot, called when user clicked 'Cancel' button
         */
        void onCancelClicked();

        /*!
         * \brief Private slot, called when user clicked 'Connect' button
         */
        void onConnectClicked();

        /*!
         * \brief Private slot, called when tracking timer events
         */
        void onTimer();

        /*!
         * \brief Private slot, called to check and enable 'Connection' button
         */
        void enableConnection();

        /*!
         * \brief Private slot, called when user changed visibiluty of password
         */
        void onShowPasswordClicked();

    private slots:
        /*!
         * \brief Private slot, called when connection to LDAP server is established
         * \param isSucceed flag, show is connection successful
         * \param errorDescription error desription string
         */
        void onConnectionCompleted(bool isSucceed, QString errorDescription);
    private:
        /*!
         * \brief Private method for load connection setttings
         */
        void loadSettings();

        /*!
         * \brief Private method for saving connection settings
         */
        void saveSettings();


        Ui::CConnectionDialog *ui{nullptr}; ///< pointer to UI implementation memeber
        CLdapSettings& m_Settings;          ///< reference to application settings
        ldapcore::CLdapData& m_LdapData;    ///< reference to ldapcore::CLdapData
        int m_WaitTime;                     ///< current value of connection time
    };
} //namespace ldapeditor

#endif // CONNECTIONDIALOG_H
