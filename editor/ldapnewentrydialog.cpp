#include <vector>
#include <string>
#include "ldapnewentrydialog.h"
#include "ui_ldapnewentrydialog.h"
#include <QListWidgetItem>
#include <QMessageBox>

std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    using namespace std;

    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos)
        {
            pos = str.length();
        }
        string token = str.substr(prev, pos - prev);
        if (!token.empty())
        {
            tokens.push_back(token);
        }
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}


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

        ui->rdnEdit->setFocus();
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

        std::string delim = "=";
        auto v = split(m_rdn.toStdString(), delim);
        if (v.size() != 2)
        {
            QMessageBox::critical(this, "Error", "The RDN has invalid format!. Format must be like as 'cn=SomaValue'!", QMessageBox::Ok);
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
