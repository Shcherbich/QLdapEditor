/*!
\file
\brief Header file for Change user's password dialog

File contains  declarations for Change user's password dialog
*/

#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
class CChangePasswordDialog;
}

namespace ldapeditor
{
    /*!
     * @ingroup ldapeditor
     * @brief The Change user's password dialog class
     *
     * The Change user's password dialog class
     */
    class CChangePasswordDialog : public QDialog
    {
        Q_OBJECT

    public:
        /*!
         * \brief constructor CChangePasswordDialog
         * \param parent pointer to parent QWidget
         */
        explicit CChangePasswordDialog(QWidget *parent = nullptr);

        /*!
         * \brief destructor CChangePasswordDialog
         */
        ~CChangePasswordDialog();

        /*!
         * \brief Method returns password
         * \return password string
         */
        QString password() const;

    private slots:
        /*!
         * \brief Method controls is possible to accept password
         */
        void enableAcceptPassword();

    private:
        Ui::CChangePasswordDialog *ui; ///< member pointer to UI implementation
    };
} //namespace ldapeditor
#endif // CHANGEPASSWORDDIALOG_H
