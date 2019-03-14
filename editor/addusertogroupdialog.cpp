/*!
@file
@brief Implementation file for CAddUserToGroupDialog class

File contains  implementations for CAddUserToGroupDialog class
*/

#include "addusertogroupdialog.h"
#include "ui_addusertogroupdialog.h"
#include "utilities.h"

namespace ldapeditor
{

/*!
 * \brief local function created lits item from user DN string
 * \param userDN user DN string
 * \return created QListWidgetItem pointer
 */
QListWidgetItem* widgetItem(const QString& userDN)
{
    QString name = userNameFromDN(userDN);
    QListWidgetItem* item = new QListWidgetItem(name);
    if(item)
        item->setData(Qt::UserRole+1, userDN);
    return item;
}



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
    searchOptions.scope = 2;  //LDAPConnection::SEARCH_SUB

    QStringList allUsers = ldapData.search(searchOptions);
    for(const QString& userDN : allUsers )
    {
        ui->allUsersList->addItem(widgetItem(userDN));
    }


    ui->addButton->setEnabled(!ui->allUsersList->selectedItems().isEmpty());
    ui->removeButton->setEnabled(!ui->groupUsersList->selectedItems().isEmpty());

    connect(ui->allUsersList, &QListWidget::itemSelectionChanged, [this](){
        ui->addButton->setEnabled(!ui->allUsersList->selectedItems().isEmpty());
    });

    connect(ui->addButton, &QPushButton::clicked, [this](){
        moveItemBettwenLists(ui->allUsersList,ui->groupUsersList);
    });

    connect(ui->groupUsersList, &QListWidget::itemSelectionChanged, [this](){
        ui->removeButton->setEnabled(!ui->groupUsersList->selectedItems().isEmpty());
    });

    connect(ui->removeButton, &QPushButton::clicked, [this](){
        moveItemBettwenLists(ui->groupUsersList, ui->allUsersList);
    });
}

CAddUserToGroupDialog::~CAddUserToGroupDialog()
{
    delete ui;
}

QStringList CAddUserToGroupDialog::membersList() const
{
    QStringList list;
    for(int i=0; i< ui->groupUsersList->count();i++)
        list << ui->groupUsersList->item(i)->data(Qt::UserRole+1).toString();
    return list;
}

void CAddUserToGroupDialog::setMembersList(const QStringList& list)
{
    for(const QString& userDN : list )
    {
        ui->groupUsersList->addItem(widgetItem(userDN));
    }
}

} //namespace ldapeditor
