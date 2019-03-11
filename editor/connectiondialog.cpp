/*!
\file
\brief Implementation  file for LDAP Connection settings class

File contains  implementations for LDAP Connection settings class
*/
#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "ldapsettings.h"
#include "ldapeditordefines.h"

#include "CLdapData.h"

#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

namespace ldapeditor
{
    CConnectionDialog::CConnectionDialog(ldapcore::CLdapData& ldapData, QWidget *parent) :
        QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowTitleHint)
        , ui(new Ui::CConnectionDialog)
        , m_LdapData(ldapData)
    {
        ui->setupUi(this);

        m_configDirPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        m_configDirPath += QString("/%1").arg(OrganizationName);

        m_sessionSettingsPath = QString("%1/%2").arg(m_configDirPath).arg("last.session");

        QSettings session(m_sessionSettingsPath, QSettings::IniFormat);
        m_lastConnectionFile = session.value("last_connection", "").toString();

        ui->connectionsCombo->setInsertPolicy(QComboBox::InsertAlphabetically);

        loadConnectionsList(m_lastConnectionFile);

        connect(ui->connectionsCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CConnectionDialog::onConnectionChanged);
        connect(ui->radioSimpleAuth, &QAbstractButton::clicked, this, &CConnectionDialog::onAuthTypeChanged);
        connect(ui->radioGssAuth, &QAbstractButton::clicked, this, &CConnectionDialog::onAuthTypeChanged);

        connect(ui->cancelButton, &QAbstractButton::clicked, this, &CConnectionDialog::onCancelClicked);
        connect(ui->connectButton, &QAbstractButton::clicked, this, &CConnectionDialog::onConnectClicked);

        connect(ui->showPasswordCheck, &QAbstractButton::clicked, [this](bool checked){
            ui->pwdEdit->setEchoMode(checked ? QLineEdit::Normal: QLineEdit::Password);
        });


        connect(&m_LdapData,  SIGNAL(onConnectionCompleted(bool, QString)), this, SLOT(onConnectionCompleted(bool, QString)));
        connect(ui->connectionsCombo, &QComboBox::editTextChanged, this, &CConnectionDialog::enableConnection );
        connect(ui->hostBox, &QLineEdit::textChanged, this, &CConnectionDialog::enableConnection );
        connect(ui->baseEdit, &QLineEdit::textChanged, this, &CConnectionDialog::enableConnection );

        ui->versionSpin->setVisible(false);
        ui->versionLabel->setVisible(false);

        loadSettings(ui->connectionsCombo->currentData().toString());

        // Hide unused controls
        ui->radioGssAuth->setVisible(false);
        ui->sslCheck->setVisible(false);
        ui->tlsCheck->setVisible(true);
        ui->saslCheck->setVisible(false);
        ui->radioSimpleAuth->setChecked(true);

        onAuthTypeChanged();
        enableConnection();

        ui->connectButton->activateWindow();
        ui->connectButton->setFocus();
    }

    CConnectionDialog::~CConnectionDialog()
    {
        delete ui;
    }


    void CConnectionDialog::loadConnectionsList(const QString& lastConnection )
    {
        QDir configDir(m_configDirPath);
        QStringList connectionsList = configDir.entryList(QStringList{"*.ini"}, QDir::Files|QDir::Readable|QDir::Writable);

        ui->connectionsCombo->addItem(tr("--New Connection--"), "");
        for(QString& strFile: connectionsList)
        {
            QSettings connectionSettings(m_configDirPath + "/" + strFile, QSettings::IniFormat);
            QString connectionName = connectionSettings.value("connection/name", "").toString();
            ui->connectionsCombo->addItem(connectionName, strFile);
        }

        int foundLastConnection = 0; //New Connection
        if(!lastConnection.isEmpty())
        {
           QString fileConnection;
           for(int i=0; i< ui->connectionsCombo->count();i++)
           {
               fileConnection = ui->connectionsCombo->itemData(i).toString();
               if( !fileConnection.isEmpty() && fileConnection == lastConnection)
               {
                   foundLastConnection = i;
                   break;
               }
           }
        }
        ui->connectionsCombo->setCurrentIndex(foundLastConnection);
        onConnectionChanged();
    }

    void CConnectionDialog::loadSettings(QString settingsFile)
    {
        if(m_Settings)
        {
            delete m_Settings;
        }

        QString filePath = m_configDirPath + "/" + settingsFile;
        m_Settings = new CLdapSettings(filePath) ;

        ui->connectionsCombo->setEditText(m_Settings->name());
        ui->hostBox->setText(m_Settings->host());
        ui->portSpin->setValue(m_Settings->port());
        ui->versionSpin->setValue(m_Settings->version());
        ui->baseEdit->setText(m_Settings->baseDN());
        ui->radioSimpleAuth->setChecked(m_Settings->simpleAuth());
        ui->sslCheck->setChecked(m_Settings->useSSL());
        ui->sslCheck->setEnabled(ui->radioSimpleAuth->isChecked());
        ui->tlsCheck->setChecked(m_Settings->useTLS());
        ui->tlsCheck->setEnabled(true);

        ui->radioGssAuth->setChecked(!ui->radioSimpleAuth->isChecked());
        ui->saslCheck->setChecked(m_Settings->useSASL());
        ui->saslCheck->setEnabled(ui->radioGssAuth->isChecked());

        ui->userEdit->setText(m_Settings->username());
        ui->pwdEdit->setText(m_Settings->password());

        ui->anonymousCheck->setChecked(m_Settings->useAnonymous());
    }

    void CConnectionDialog::saveSettings()
    {

        QString fName =  ui->connectionsCombo->currentData().toString();
        if(fName.isEmpty())
        {
            QString fDesc = ui->connectionsCombo->currentText();
            fName = fDesc.isEmpty() ? "New Connection.ini" : fDesc + ".ini";
        }
        QString iniPath = m_configDirPath + "/" + fName;
        CLdapSettings* tmpSettings = new CLdapSettings(iniPath);

        tmpSettings->setName(ui->connectionsCombo->currentText());
        tmpSettings->setHost(ui->hostBox->text());
        tmpSettings->setPort(ui->portSpin->value());
        tmpSettings->setVersion(ui->versionSpin->value());
        tmpSettings->setBaseDN(ui->baseEdit->text());

        tmpSettings->setSimpleAuth(ui->radioSimpleAuth->isChecked());
        tmpSettings->setUseSSL(ui->sslCheck->isChecked());
        tmpSettings->setUseTLS(ui->tlsCheck->isChecked());
        tmpSettings->setUseSASL(ui->saslCheck->isChecked());

        tmpSettings->setUsername(ui->userEdit->text());
        tmpSettings->setPassword(ui->pwdEdit->text());
        tmpSettings->setUseAnonymous(ui->anonymousCheck->isChecked());

        tmpSettings->saveSettings();

        m_lastConnectionFile = fName;

        int index = ui->connectionsCombo->findText(fName);
        if(index == -1)
        {
            // add new connection to list
            ui->connectionsCombo->addItem(fName, fName);
            ui->connectionsCombo->model()->sort(0);

            // find and set new added connection as current
            index = ui->connectionsCombo->findText(fName);
            ui->connectionsCombo->setCurrentIndex(index);
        }

        delete m_Settings;
        m_Settings = tmpSettings;
    }

    void CConnectionDialog::onAuthTypeChanged()
    {
        bool simpleAuth = ui->radioSimpleAuth->isChecked();

        ui->sslCheck->setEnabled(simpleAuth);
        ui->tlsCheck->setEnabled(true);
        ui->saslCheck->setEnabled(!simpleAuth);

        ui->anonymousCheck->setText(simpleAuth
                                    ? tr("Anonymous connection")
                                    : tr("Use current user credentials")
                                      );
    }

    void CConnectionDialog::onConnectionChanged()
    {
        int currentIdx = ui->connectionsCombo->currentIndex();
        if(ui->connectionsCombo->itemData(currentIdx).toString().isEmpty())
            ui->connectionsCombo->setCurrentText("");

        loadSettings(ui->connectionsCombo->currentData().toString());
    }

    void CConnectionDialog::onCancelClicked()
    {
        reject();
    }

    void CConnectionDialog::onConnectClicked()
    {
        saveSettings();        
        setEnabled(false);
        m_WaitTime = m_Settings->timeout();
        setWindowTitle(QString("Connecting ... %1s").arg(0));

        m_LdapData.connect(m_Settings->connectionOptions());
        QTimer::singleShot(1000, this, &CConnectionDialog::onTimer );
    }

    void CConnectionDialog::onTimer()
    {
        if(--m_WaitTime > 0)
        {
            setWindowTitle(QString("Connecting ... %1s").arg(m_Settings->timeout() - m_WaitTime));
            QTimer::singleShot(1000, this, &CConnectionDialog::onTimer );
        }
        else
        {
            setWindowTitle(tr("Connection properties"));
            setEnabled(true);
        }
    }

    void CConnectionDialog::onConnectionCompleted(bool isSucceed, QString errorDescription)
    {
        if(isSucceed)
        {
            m_WaitTime = 0;
            setWindowTitle(tr("Connection properties"));
            setEnabled(true);

            QSettings session(m_sessionSettingsPath, QSettings::IniFormat);
            session.setValue("last_connection", m_lastConnectionFile);
            accept();
        }
        else
        {
            m_WaitTime = 0;
            QMessageBox::critical(this, tr("Connection fails"), errorDescription, QMessageBox::Ok);
        }
    }

    void CConnectionDialog::enableConnection()
    {
        bool enableConnection = !ui->connectionsCombo->currentText().trimmed().isEmpty() &&
                                !ui->hostBox->text().trimmed().isEmpty() &&
                                !ui->baseEdit->text().trimmed().isEmpty();
        ui->connectButton->setEnabled(enableConnection);
    }

} // namespace ldapeditor
