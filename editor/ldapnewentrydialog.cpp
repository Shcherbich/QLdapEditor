#include "ldapnewentrydialog.h"
#include "ui_ldapnewentrydialog.h"
#include <QListWidgetItem>
#include <QMessageBox>

namespace ldapeditor
{
    CLdapNewEntryDialog::CLdapNewEntryDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CLdapNewEntryDialog)

    {
        ui->setupUi(this);
    }

    CLdapNewEntryDialog::CLdapNewEntryDialog(QWidget *parent, QString parentDn, QVector<QString> vAllClasses) :
        QDialog(parent),
        ui(new Ui::CLdapNewEntryDialog)

    {
        ui->setupUi(this);

        connect(ui->closeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onCloseClicked);
        connect(ui->okButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onOkClicked);
        connect(ui->addButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onAddClicked);
        connect(ui->removeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onRemoveClicked);

        this->vAllClasses.swap(vAllClasses);
        ui->parentdnEdit->setText(parentDn);
        ui->parentdnEdit->setReadOnly(true);
        for(auto c : this->vAllClasses)
        {
            ui->listAll->addItem(c);
        }
    }

    CLdapNewEntryDialog::~CLdapNewEntryDialog()
    {
        delete ui;
    }

    void CLdapNewEntryDialog::onCloseClicked()
    {
        reject();
    }

    void CLdapNewEntryDialog::onOkClicked()
    {
        m_rdn = ui->rdnEdit->text().trimmed();
        if (m_rdn.size() == 0)
        {
            QMessageBox::critical(this, "Error", "The RDN is empty!", QMessageBox::Ok);
            ui->rdnEdit->setFocus();
            return;
        }
        if (ui->listNeeded->count() == 0)
        {
            QMessageBox::critical(this, "Error", "The selected classes are empty!\nPlease, Choose items.", QMessageBox::Ok);
            return;
        }
        for(int i = 0; i < ui->listNeeded->count(); ++i)
        {
            vSelectedClasses << ui->listNeeded->item(i)->text();
        }

        accept();
    }

    void CLdapNewEntryDialog::onAddClicked()
    {
        QList<QListWidgetItem*> l = ui->listAll->selectedItems();
        if (l.size() == 0)
        {
            return;
        }
        QListWidgetItem* item = l.first();
        ui->listNeeded->addItem(item->text());
        delete ui->listAll->takeItem(ui->listAll->row(item));
    }

    void CLdapNewEntryDialog::onRemoveClicked()
    {
        QList<QListWidgetItem*> l = ui->listNeeded->selectedItems();
        if (l.size() == 0)
        {
            return;
        }
        QListWidgetItem* item = l.first();
        ui->listAll->addItem(item->text());
        delete ui->listNeeded->takeItem(ui->listNeeded->row(item));
    }

    QString CLdapNewEntryDialog::rdn() const
    {
        return m_rdn;
    }

    QVector<QString> CLdapNewEntryDialog::selectedClasses() const
    {
        return vSelectedClasses;
    }


}//namespace ldapeditor
