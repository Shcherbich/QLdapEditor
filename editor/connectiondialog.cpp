#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "ldapsettings.h"

#include "CLdapData.h"

#include <QSettings>
#include <QTimer>
#include <QMessageBox>

namespace ldapeditor
{
    CConnectionDialog::CConnectionDialog(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget *parent) :
        QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowTitleHint)
        , ui(new Ui::CConnectionDialog)
        , m_Settings(settings)
        , m_LdapData(ldapData)
    {
        ui->setupUi(this);

        connect(ui->radioSimpleAuth, &QAbstractButton::clicked, this, &CConnectionDialog::onAuthTypeChanged);
        connect(ui->radioGssAuth, &QAbstractButton::clicked, this, &CConnectionDialog::onAuthTypeChanged);

        connect(ui->cancelButton, &QAbstractButton::clicked, this, &CConnectionDialog::onCancelClicked);
        connect(ui->connectButton, &QAbstractButton::clicked, this, &CConnectionDialog::onConnectClicked);
        connect(ui->showPasswordCheck, &QAbstractButton::clicked, this, &CConnectionDialog::onShowPasswordClicked);

        connect(&m_LdapData,  &ldapcore::CLdapData::OnConnectionCompleted, this, &CConnectionDialog::OnConnectionCompleted);
        connect(ui->hostBox, &QLineEdit::textChanged, this, &CConnectionDialog::enableConnection );
        connect(ui->baseEdit, &QLineEdit::textChanged, this, &CConnectionDialog::enableConnection );

        ui->versionSpin->setVisible(false);
        ui->versionLabel->setVisible(false);

        loadSettings();

        // Hide unused controls
        ui->radioGssAuth->setVisible(false);
        ui->sslCheck->setVisible(false);
        ui->tlsCheck->setVisible(false);
        ui->saslCheck->setVisible(false);
        ui->radioSimpleAuth->setChecked(true);

        onAuthTypeChanged();
        enableConnection();
    }

    CConnectionDialog::~CConnectionDialog()
    {
        delete ui;
    }

    void CConnectionDialog::loadSettings()
    {
        ui->connectionNameEdit->setText(m_Settings.name());
        ui->hostBox->setText(m_Settings.host());
        ui->portSpin->setValue(m_Settings.port());
        ui->versionSpin->setValue(m_Settings.version());
        ui->baseEdit->setText(m_Settings.baseDN());
        ui->radioSimpleAuth->setChecked(m_Settings.simpleAuth());
        ui->sslCheck->setChecked(m_Settings.useSSL());
        ui->sslCheck->setEnabled(ui->radioSimpleAuth->isChecked());
        ui->tlsCheck->setChecked(m_Settings.useTLS());
        ui->tlsCheck->setEnabled(ui->radioSimpleAuth->isChecked());

        ui->radioGssAuth->setChecked(!ui->radioSimpleAuth->isChecked());
        ui->saslCheck->setChecked(m_Settings.useSASL());
        ui->saslCheck->setEnabled(ui->radioGssAuth->isChecked());

        ui->userEdit->setText(m_Settings.username());
        ui->pwdEdit->setText(m_Settings.password());

        ui->anonymousCheck->setChecked(m_Settings.useAnonymous());

    }

    void CConnectionDialog::saveSettings()
    {
        m_Settings.setName(ui->connectionNameEdit->text());
        m_Settings.setHost(ui->hostBox->text());
        m_Settings.setPort(ui->portSpin->value());
        m_Settings.setVersion(ui->versionSpin->value());
        m_Settings.setBaseDN(ui->baseEdit->text());

        m_Settings.setSimpleAuth(ui->radioSimpleAuth->isChecked());
        m_Settings.setUseSSL(ui->sslCheck->isChecked());
        m_Settings.setUseTLS(ui->tlsCheck->isChecked());
        m_Settings.setUseSASL(ui->saslCheck->isChecked());

        m_Settings.setUsername(ui->userEdit->text());
        m_Settings.setPassword(ui->pwdEdit->text());

        m_Settings.setUseAnonymous(ui->anonymousCheck->isChecked());

        m_Settings.sync();
    }

    void CConnectionDialog::onAuthTypeChanged()
    {
        bool simpleAuth = ui->radioSimpleAuth->isChecked();

        ui->sslCheck->setEnabled(simpleAuth);
        ui->tlsCheck->setEnabled(simpleAuth);
        ui->saslCheck->setEnabled(!simpleAuth);

        ui->anonymousCheck->setText(simpleAuth
                                    ? tr("Anonymous connection")
                                    : tr("Use current user credentials")
                                      );
    }

    void CConnectionDialog::onCancelClicked()
    {
        reject();
    }

    void CConnectionDialog::onConnectClicked()
    {
        saveSettings();        
        setEnabled(false);
        m_WaitTime = m_Settings.timeout();
        setWindowTitle(QString("Connecting ... %1s").arg(0));

        m_LdapData.connect(m_Settings.connectionOptions());
        accept();
        //QTimer::singleShot(1000, this, &CConnectionDialog::onTimer );
    }

    void CConnectionDialog::onTimer()
    {
        if(--m_WaitTime > 0)
        {
            setWindowTitle(QString("Connecting ... %1s").arg(m_Settings.timeout() - m_WaitTime));
            QTimer::singleShot(1000, this, &CConnectionDialog::onTimer );
        }
        else
        {
            setWindowTitle(tr("Connection properties dialog"));
            setEnabled(true);
        }
    }

    void CConnectionDialog::OnConnectionCompleted(ldapcore::CLdapData* pThis, bool isSucceed, QString errorDescription)
    {
        if(isSucceed)
        {
            setWindowTitle(tr("Connection properties dialog"));
            setEnabled(true);
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
        bool enableConnection= !ui->hostBox->text().trimmed().isEmpty() && !ui->baseEdit->text().trimmed().isEmpty();
        ui->connectButton->setEnabled(enableConnection);
    }

    void CConnectionDialog::onShowPasswordClicked()
    {
        ui->pwdEdit->setEchoMode(ui->showPasswordCheck->isChecked() ? QLineEdit::Normal: QLineEdit::Password);
    }
} // namespace ldapeditor
