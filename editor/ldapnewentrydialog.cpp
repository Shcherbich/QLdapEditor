/*!
\file
\brief Implementation file for dialog 'Add/Edit LDAP Entity' class

File contains  implementations for dialog 'Add/EditLDPA Entity' class
*/

#include "ldapnewentrydialog.h"
#include "ui_ldapnewentrydialog.h"
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

    connect(ui->closeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onCloseClicked);
    connect(ui->okButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onOkClicked);
    connect(ui->addButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onAddClicked);
    connect(ui->removeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onRemoveClicked);
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

    ui->addButton->setEnabled(false);
    ui->removeButton->setEnabled(false);

    connect(ui->closeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onCloseClicked);
    connect(ui->okButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onOkClicked);
    connect(ui->addButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onAddClicked);
    connect(ui->removeButton, &QAbstractButton::clicked, this, &CLdapNewEntryDialog::onRemoveClicked);


    connect(ui->listAll, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem* previous){
        Q_UNUSED(previous);
        ui->addButton->setEnabled(current != nullptr);
    });
    connect(ui->listNeeded, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem* previous){
        Q_UNUSED(previous);
        ui->removeButton->setEnabled(current != nullptr);
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

void CLdapNewEntryDialog::onAddClicked()
{
    QList<QListWidgetItem*> l = ui->listAll->selectedItems();
    if (!l.isEmpty())
    {
        QListWidgetItem* item = l.first();
        ui->listNeeded->addItem(item->text());
        delete ui->listAll->takeItem(ui->listAll->row(item));
    }    
}

void CLdapNewEntryDialog::onRemoveClicked()
{
    QList<QListWidgetItem*> l = ui->listNeeded->selectedItems();
    if (!l.isEmpty())
    {
        QListWidgetItem* item = l.first();
        ui->listAll->addItem(item->text());
        delete ui->listNeeded->takeItem(ui->listNeeded->row(item));
    }
}

QString CLdapNewEntryDialog::rdn() const
{
    return m_rdn;
}

QVector<QString> CLdapNewEntryDialog::selectedClasses() const
{
    return vSelectedClasses;
}

void CLdapNewEntryDialog::onStructuralComboChanged(const QString&)
{
    //ui->rdnEdit->setText("");
    QString selected = ui->structuralCombo->currentText();
    auto& schema = m_LdapData.schema();

    auto startRdn = schema.startRdn(selected);
    if (!startRdn.isEmpty())
    {
        ui->rdnEdit->setText(startRdn + "=");
        ui->rdnEdit->setCursorPosition(ui->rdnEdit->selectedText().size() - 1);
    }

    fillListAll(selected);
}

void CLdapNewEntryDialog::fillListAll(QString structuralClass)
{
    auto& schema = m_LdapData.schema();
    QString selected = structuralClass;
    QVector<QString> v;
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
        ui->listAll->addItem(c);
}

QString CLdapNewEntryDialog::structuralClass() const
{
    return m_structuralClass;
}

} //namespace ldapeditor
