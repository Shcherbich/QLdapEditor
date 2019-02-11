#include "mainwindow.h"
#include "connectiondialog.h"
#include "ldapsettings.h"
#include "CLdapData.h"

#include <QApplication>
#include <QSettings>
#include <signal.h>

int main(int argc, char *argv[])
{
    // Do by default to ignore this interrupt
    ::signal(SIGPIPE, SIG_IGN);

    QApplication a(argc, argv);

    ldapeditor::CLdapSettings settings;
    ldapcore::CLdapData ldapData;
    ldapeditor::CConnectionDialog connectionDlg(settings, ldapData);
    if(connectionDlg.exec() == QDialog::Rejected)
    {
        return 0;
    }

    ldapeditor::MainWindow w(settings, ldapData);
    w.show();
    return a.exec();
}
