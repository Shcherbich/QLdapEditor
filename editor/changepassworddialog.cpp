/*!
\file
\brief Implementation file for Change user's password dialog

File contains  imple,entations for Change user's password dialog
*/

#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"
#include <QPushButton>

namespace ldapeditor
{

CChangePasswordDialog::CChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CChangePasswordDialog)
{
    ui->setupUi(this);

    connect(ui->showPassword, &QAbstractButton::clicked, [this](bool checked){
        QLineEdit::EchoMode mode = checked ? QLineEdit::Normal: QLineEdit::Password;
        ui->passwordEdit->setEchoMode(mode);
        ui->passwordEdit2->setEchoMode(mode);
    });

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(ui->passwordEdit, &QLineEdit::textChanged, this, &CChangePasswordDialog::enableAcceptPassword);
    connect(ui->passwordEdit2, &QLineEdit::textChanged, this, &CChangePasswordDialog::enableAcceptPassword);
}

CChangePasswordDialog::~CChangePasswordDialog()
{
    delete ui;
}

void CChangePasswordDialog::enableAcceptPassword()
{
        QString pwd = ui->passwordEdit->text();
        QString pwd2 = ui->passwordEdit2->text();
        bool bAccept = !pwd.isEmpty() && (pwd == pwd2);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(bAccept);
};

QString CChangePasswordDialog::password() const
{
    return ui->passwordEdit->text();
}

}// namespace ldapeditor
