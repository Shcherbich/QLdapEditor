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
    class CConnectionDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit CConnectionDialog(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget *parent = nullptr);
        ~CConnectionDialog();
    private slots:
        void onAuthTypeChanged();
        void onCancelClicked();
        void onConnectClicked();
        void onTimer();

        void OnConnectionCompleted(ldapcore::CLdapData* pThis, bool isSucceed, QString errorDescription);
        void enableConnection();

    private:
        void loadSettings();
        void saveSettings();


        Ui::CConnectionDialog *ui{nullptr};
        CLdapSettings& m_Settings;
        ldapcore::CLdapData& m_LdapData;
        int m_WaitTime;
    };
} //namespace ldapeditor

#endif // CONNECTIONDIALOG_H
