/*!
\file
\brief Implementation file for dialog 'Add/Edit LDAP Entity' class

File contains  implementations for dialog 'Add/EditLDPA Entity' class
*/

#include "ldapnewentrydialog.h"
#include "ui_ldapnewentrydialog.h"
#include "utilities.h"

#include <QComboBox>
#include <QListWidgetItem>
#include <QMessageBox>
#include <string>
#include <vector>

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
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

namespace ldapeditor {

CLdapNewEntryDialog::CLdapNewEntryDialog(QWidget* parent, QString parentDn, ldapcore::CLdapData& ldapData)
    : QDialog(parent)
    , ui(new Ui::CLdapNewEntryDialog)
    , m_LdapData(ldapData)

{
    ui->setupUi(this);

    ui->listAll->setSortingEnabled(true);
    ui->listNeeded->setSortingEnabled(true);

    ui->addButton->setEnabled(false);
    ui->removeButton->setEnabled(false);

    connect(ui->closeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onCloseClicked);
    connect(ui->okButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onOkClicked);

    connect(ui->addButton, &QAbstractButton::clicked, [this](){
         moveItemBettwenLists(ui->listAll, ui->listNeeded);
    });

    connect(ui->removeButton, &QAbstractButton::clicked, [this](){
         moveItemBettwenLists(ui->listNeeded, ui->listAll);
    });

    connect(ui->listAll, &QListWidget::itemSelectionChanged, this, [this](){
        ui->addButton->setEnabled(!ui->listAll->selectedItems().isEmpty());
    });

    connect(ui->listNeeded, &QListWidget::itemSelectionChanged, this, [this](){
        ui->removeButton->setEnabled(!ui->listNeeded->selectedItems().isEmpty());
    });
    connect(ui->structuralCombo, &QComboBox::currentTextChanged, this, &CLdapNewEntryDialog::onStructuralComboChanged);

    ui->parentdnEdit->setText(parentDn);
    ui->parentdnEdit->setReadOnly(true);
    for (auto c : m_LdapData.schema().structuralClasses())
    {
        ui->structuralCombo->addItem(c);
    }

    ui->rdnEdit->setFocus();
}

CLdapNewEntryDialog::CLdapNewEntryDialog(QWidget* parent, QString dn, QString rdn,
    std::string& structuralClass,
    std::vector<std::string>& auxClasses,
    ldapcore::CLdapData& ldapData)
    : QDialog(parent)
    , ui(new Ui::CLdapNewEntryDialog)
    , m_LdapData(ldapData)
    , m_editMode(true)

{
    ui->setupUi(this);
    ui->listAll->setSortingEnabled(true);
    ui->listNeeded->setSortingEnabled(true);

    ui->addButton->setEnabled(false);
    ui->removeButton->setEnabled(false);

    connect(ui->closeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onCloseClicked);
    connect(ui->okButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onOkClicked);

    connect(ui->addButton, &QAbstractButton::clicked, [this](){
         moveItemBettwenLists(ui->listAll, ui->listNeeded);
    });

    connect(ui->removeButton, &QAbstractButton::clicked, [this](){
         moveItemBettwenLists(ui->listNeeded, ui->listAll);
    });

    connect(ui->listAll, &QListWidget::itemSelectionChanged, this, [this](){
        ui->addButton->setEnabled(!ui->listAll->selectedItems().isEmpty());
    });

    connect(ui->listNeeded, &QListWidget::itemSelectionChanged, this, [this](){
        ui->removeButton->setEnabled(!ui->listNeeded->selectedItems().isEmpty());
    });

    ui->structuralCombo->addItem(structuralClass.c_str());
    ui->structuralCombo->setEnabled(false);
    ui->parentdnEdit->setText(dn);
    ui->rdnEdit->setText(rdn);
    ui->parentdnEdit->setReadOnly(true);
    ui->rdnEdit->setReadOnly(true);
    setWindowTitle(tr("Edit entry"));

    fillListAll(structuralClass.c_str());

    for (auto& c : auxClasses)
        ui->listNeeded->addItem(c.c_str());

    auxClasses.push_back(structuralClass);
    for (auto& c : auxClasses)
    {
        for (int i = 0; i < ui->listAll->count(); ++i)
        {
            if (ui->listAll->item(i)->text() == c.c_str())
            {
                delete ui->listAll->takeItem(ui->listAll->row(ui->listAll->item(i)));
                break;
            }
        }
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
    if (m_rdn.size() == 0 && !m_editMode)
    {
        QMessageBox::critical(this, tr("Error"), tr("The RDN is empty!"), QMessageBox::Ok);
        ui->rdnEdit->setFocus();
        return;
    }

    std::string delim = "=";
    auto v = split(m_rdn.toStdString(), delim);
    if (v.size() != 2 && !m_editMode)
    {
        QMessageBox::critical(this, tr("Error"), tr("The RDN has invalid format!. Format must be like as 'cn=SomaValue'!"), QMessageBox::Ok);
        return;
    }

    vSelectedClasses.clear();
    QString structuralClass = ui->structuralCombo->currentText();
//    if (structuralClass != "top")
//    {
//        vSelectedClasses << "top";
//    }
//    vSelectedClasses << structuralClass;
    m_structuralClass = structuralClass;
    for (int i = 0; i < ui->listNeeded->count(); ++i)
    {
        vSelectedClasses << ui->listNeeded->item(i)->text();
    }

    accept();
}

QString CLdapNewEntryDialog::rdn() const
{
    return m_rdn;
}

QStringList CLdapNewEntryDialog::selectedClasses() const
{
    return vSelectedClasses;
}

void CLdapNewEntryDialog::onStructuralComboChanged(const QString&)
{
    //ui->rdnEdit->setText("");
    QString selected = ui->structuralCombo->currentText();
    auto& schema = m_LdapData.schema();

    QString rdnValue = ui->rdnEdit->text();
    auto startRdn = schema.startRdn(selected);
    if (!startRdn.isEmpty())
    {        
        ui->rdnEdit->setText(startRdn + "=" + rdnValue.mid(rdnValue.indexOf('=')+1));
        ui->rdnEdit->setCursorPosition(ui->rdnEdit->selectedText().size() - 1);
    }

    fillListAll(selected);
}

void CLdapNewEntryDialog::fillListAll(QString structuralClass)
{
    auto& schema = m_LdapData.schema();
    QString selected = structuralClass;
    QStringList v;
    QString sup;
    do
    {
        sup = schema.supByClass(selected);
        v << schema.auxiliaryClassesBySup(sup);
        selected = sup == selected ? "" : sup;
    } while (!selected.isEmpty());

    qSort(v);
    v.erase(std::unique(v.begin(), v.end()), v.end());    
    ui->listAll->clear();
    ui->listNeeded->clear();

    for (auto& c : v)
    {
        QListWidgetItem* item = new QListWidgetItem(c);
        ui->listAll->addItem(item);
    }

}

QString CLdapNewEntryDialog::structuralClass() const
{
    return m_structuralClass;
}

} //namespace ldapeditor
