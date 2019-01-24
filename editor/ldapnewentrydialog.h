#ifndef CLdapNewEntryDialog_H
#define CLdapNewEntryDialog_H

#include <QString>
#include <QVector>
#include <QDialog>

namespace Ui {
class CLdapNewEntryDialog;
}

namespace ldapeditor
{
    class CLdapNewEntryDialog : public QDialog
    {
        Q_OBJECT

    public:
        explicit CLdapNewEntryDialog(QWidget *parent = nullptr);
        explicit CLdapNewEntryDialog(QWidget *parent, QString parentDn, QVector<QString> vAllClasses);
        ~CLdapNewEntryDialog();

        QString rdn() const;
        QVector<QString> selectedClasses() const;

    private slots:
        void onCloseClicked();
        void onOkClicked();
        void onAddClicked();
        void onRemoveClicked();

    private:
        Ui::CLdapNewEntryDialog* ui;
        QVector<QString> vAllClasses;
        QString          m_rdn;
        QVector<QString> vSelectedClasses;
    };
}//namespace ldapeditor

#endif // CLdapNewEntryDialog_H
