/*!
@file
@brief Implementation file for CAddUserToGroupDialog class

File contains  implementations for CAddUserToGroupDialog class
*/

#include "addusertogroupdialog.h"
#include "ui_addusertogroupdialog.h"

namespace ldapeditor
{
CAddUserToGroupDialog::CAddUserToGroupDialog(ldapcore::CLdapData &ldapData, CLdapSettings& settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAddUserToGroupDialog)
{
    ui->setupUi(this);

    ui->allUsersList->setSortingEnabled(true);
    ui->groupUsersList->setSortingEnabled(true);

    ldapcore::tSearchOptions searchOptions;
    searchOptions.basedn = settings.findUsersBaseDN().toStdString() ;
    searchOptions.filter = settings.findUsersFilter().toStdString();//"objectClass=user";
    searchOptions.attributes = "dn";
    searchOptions.scope = 1;  //LDAPConnection::SEARCH_ONE

    QStringList allUsers = ldapData.search(searchOptions);
    ui->allUsersList->addItems(allUsers);

    ui->addButton->setEnabled(!ui->allUsersList->selectedItems().isEmpty());
    connect(ui->allUsersList, &QListWidget::itemSelectionChanged, this, &CAddUserToGroupDialog::onAllUsersSelectionChanged);
    connect(ui->addButton, &QPushButton::clicked, this, &CAddUserToGroupDialog::onAddUserToGroup);

    ui->removeButton->setEnabled(!ui->groupUsersList->selectedItems().isEmpty());
    connect(ui->groupUsersList, &QListWidget::itemSelectionChanged, this, &CAddUserToGroupDialog::onGroupUsersSelectionChanged);
    connect(ui->removeButton, &QPushButton::clicked, this, &CAddUserToGroupDialog::onRemoveUserFromGroup);
}

CAddUserToGroupDialog::~CAddUserToGroupDialog()
{
    delete ui;
}

void CAddUserToGroupDialog::onAllUsersSelectionChanged()
{
    ui->addButton->setEnabled(!ui->allUsersList->selectedItems().isEmpty());
}

void CAddUserToGroupDialog::onAddUserToGroup()
{
    for(auto i: ui->allUsersList->selectedItems())
    {
        ui->groupUsersList->addItem(i->text());
    }
    qDeleteAll(ui->allUsersList->selectedItems());
}

void CAddUserToGroupDialog::onGroupUsersSelectionChanged()
{
    ui->removeButton->setEnabled(!ui->groupUsersList->selectedItems().isEmpty());
}

void CAddUserToGroupDialog::onRemoveUserFromGroup()
{
    for(auto i: ui->groupUsersList->selectedItems())
    {
        ui->allUsersList->addItem(i->text());
    }
    qDeleteAll(ui->groupUsersList->selectedItems());
}

QStringList CAddUserToGroupDialog::membersList() const
{
    QStringList list;
    for(int i=0; i< ui->groupUsersList->count();i++)
        list << ui->groupUsersList->item(i)->text();
    return list;
}
void CAddUserToGroupDialog::setMembersList(const QStringList& list)
{
    ui->groupUsersList->addItems(list);
}

} //namespace ldapeditor
