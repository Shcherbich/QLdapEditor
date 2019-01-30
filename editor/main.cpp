#include "mainwindow.h"
#include "connectiondialog.h"
#include "ldapsettings.h"
#include "CLdapData.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
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
