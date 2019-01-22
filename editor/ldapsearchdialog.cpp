#include "ldapsearchdialog.h"
#include "ui_ldapsearchdialog.h"
#include "CLdapData.h"

#include <QTableWidgetItem>

namespace ldapeditor
{
    CLDapSearchDialog::CLDapSearchDialog(ldapcore::CLdapData& ldapData, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CLDapSearchDialog)
       , m_LdapData(ldapData)
    {
        ui->setupUi(this);
        ui->searchDnEdit->setText(ldapData.baseDN());
        ui->filterEdit->setText("objectClass=*");

        ui->radioSubTree->setChecked(true);
        ui->label_4->setVisible(false);
        ui->radioOneLevel->setVisible(false);
        ui->radioSubTree->setVisible(false);

        connect(ui->closeButton, &QAbstractButton::clicked, this, &CLDapSearchDialog::onCloseClicked);
        connect(ui->searchButton, &QAbstractButton::clicked, this, &CLDapSearchDialog::onSearchClicked);

        connect(ui->searchDnEdit, &QLineEdit::textChanged, this, &CLDapSearchDialog::enableSearch );
        connect(ui->filterEdit, &QLineEdit::textChanged, this, &CLDapSearchDialog::enableSearch );
        connect(ui->attrEdit, &QLineEdit::textChanged, this, &CLDapSearchDialog::enableSearch );

        enableSearch();
    }

    CLDapSearchDialog::~CLDapSearchDialog()
    {
        delete ui;
    }

    void CLDapSearchDialog::onCloseClicked()
    {
        accept();
    }

    void CLDapSearchDialog::onSearchClicked()
    {
        ui->tableWidget->clear();

        ldapcore::tSearchOptions searchOptions;
        searchOptions.basedn = ui->searchDnEdit->text().toStdString();
        searchOptions.filter = ui->filterEdit->text().toStdString();
        searchOptions.attributes = "dn, ";
        searchOptions.attributes += ui->attrEdit->text().toStdString();
        searchOptions.scope = ui->radioOneLevel->isChecked()
                ? 1  //LDAPConnection::SEARCH_ONE
                : 2 ;//LDAPConnection::SEARCH_SUB

        QStringList headers;
        for(const auto& h : QString::fromStdString(searchOptions.attributes).split(","))
        {
            headers << h.trimmed();
        }
        QStringList items = m_LdapData.search(searchOptions);

        ui->tableWidget->setRowCount(items.size());
        ui->tableWidget->setColumnCount(headers.size());
        ui->tableWidget->setHorizontalHeaderLabels(headers);

        for(int row=0; row < items.size(); row++)
        {
            QStringList rowValues = prepareResultRow(items[row], headers);
            for(int column=0; column < rowValues.size(); column++)
            {
                QTableWidgetItem* item = new QTableWidgetItem(rowValues[column]);
                ui->tableWidget->setItem(row, column, item);
            }
        }
    }
    void CLDapSearchDialog::enableSearch()
    {
        bool enableSearch = !ui->searchDnEdit->text().trimmed().isEmpty() &&
                            !ui->filterEdit->text().trimmed().isEmpty() &&
                            !ui->attrEdit->text().trimmed().isEmpty();
        ui->searchButton->setEnabled(enableSearch);
    }

    QStringList CLDapSearchDialog::prepareResultRow(const QString& dn, const QStringList& headers)
    {
        QStringList rowValues(dn);
        QStringList columns = dn.split(",");
        for(int column=1; column < columns.size(); column++)
        {
            rowValues << "";
        }

        for(int column=0; column < columns.size(); column++)
         {
             const QString& attr = columns[column];
             QStringList values = attr.split("=");
             const QString& key = values[0].trimmed();
             const QString& value = values[1].trimmed();

             // find column for data
             for(int i=1;i<headers.size();i++)
             {
                 if(key.compare(headers[i], Qt::CaseInsensitive) == 0)
                 {
                     rowValues[i] = value;
                 }
             }
         }
        return rowValues;
    }
}//namespace ldapeditor
