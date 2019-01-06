#ifndef LDAPSEARCHDIALOG_H
#define LDAPSEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class CLDapSearchDialog;
}

namespace ldapcore {
class CLdapData;
}

namespace ldapeditor
{
    class CLDapSearchDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit CLDapSearchDialog(ldapcore::CLdapData &ldapData, QWidget *parent = nullptr);
        ~CLDapSearchDialog();
    private slots:
        void onCloseClicked();
        void onSearchClicked();

    private:
        Ui::CLDapSearchDialog *ui;
        ldapcore::CLdapData& m_LdapData;
        void enableSearch();
        QStringList prepareResultRow(const QString &dn, const QStringList &headers);
    };
}//namespace ldapeditor

#endif // LDAPSEARCHDIALOG_H
