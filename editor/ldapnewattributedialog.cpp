/*!
\file
\brief Implementation file for dialog 'Add Attribute' class

File contains  implementations for dialog 'Add Attribute' class
*/

#include "ldapnewattributedialog.h"
#include "ui_ldapnewattributedialog.h"
#include "attributemodelhelper.h"
#include <QDateTime>

namespace ldapeditor
{

CLdapNewAttributeDialog::CLdapNewAttributeDialog(ldapcore::CLdapData& ldapData, ldapcore::CLdapEntry* entry, QWidget* parent) :
    QDialog(parent), m_LdapData(ldapData), m_entry(entry),
    ui(new Ui::CLdapNewAttributeDialog)
{
    ui->setupUi(this);

    connect(ui->classCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentClassChanged(int)));
    connect(ui->attrCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentAttributeChanged(int)));

    ui->classCombo->setStyleSheet("QComboBox{ padding-left: 6px; }");
    ui->attrCombo->setStyleSheet("QComboBox{ padding-left: 6px; }");
    ui->typeEdit->setStyleSheet("QLineEdit{ padding-left: 6px; }");
    ui->valueEdit->setStyleSheet("QLineEdit{ padding-left: 6px; }");


    ui->typeEdit->setReadOnly(true);

    ui->labelValue->hide();
    ui->valueEdit->hide();

    QString structuralClass = m_entry->structuralClass();
    QStringList entryClasses = m_entry->classes();
    QStringList classes = m_LdapData.schema().consistentClassesByStructuralAndOther(structuralClass, entryClasses);

    int structuraClassIdx = -1;
    for (int i = 0; i < classes.size(); i++)
    {
        ui->classCombo->addItem(classes[i]);
        if (m_entry->structuralClass() == classes[i])
        {
            structuraClassIdx = i;
        }
    }

    if (structuraClassIdx != -1)
    {
        ui->classCombo->setCurrentIndex(structuraClassIdx);
    }

    onCurrentClassChanged(structuraClassIdx);
}

CLdapNewAttributeDialog::~CLdapNewAttributeDialog()
{
    delete ui;
}

void CLdapNewAttributeDialog::onCurrentClassChanged(int index)
{
    ui->attrCombo->setEnabled(index != -1);
    if (index != -1)
    {
        std::map<std::string, std::string> a2v;
        QStringList classes{ui->classCombo->currentText()};
        m_attributes = m_LdapData.schema().attributeByClasses(classes, a2v);
        ui->attrCombo->clear();

        qSort(m_attributes.begin(), m_attributes.end(), [](const ldapcore::CLdapAttribute & a1, const ldapcore::CLdapAttribute & a2)
        {
            return a1.name().toLower() < a2.name().toLower();
        });

        QSet<QString> attrSet;
        QVector<ldapcore::CLdapAttribute>* entryAttributes = m_entry->attributes();
        for (const auto& a : *entryAttributes)
        {
            attrSet.insert(a.name());
        }

        for (int i = 0; i < m_attributes.size(); i++)
        {
            // not this check is disabled, because some attributes can have multiple values
            //if(!attrSet.contains(m_attributes[i].name()))
            {
                ui->attrCombo->addItem(m_attributes[i].name(), i);
            }
        }

        ui->attrCombo->setCurrentIndex(0);
        onCurrentAttributeChanged(0);
    }
}

void CLdapNewAttributeDialog::onCurrentAttributeChanged(int index)
{
    if (index != -1)
    {
        CAttributeModelHelper helper(m_LdapData);
        int attrIdx = ui->attrCombo->itemData(index).toInt();

        ldapcore::AttrType type = m_attributes[attrIdx].type();
        QString value = m_attributes[attrIdx].value();

        ui->typeEdit->setText(helper.attributeType2String(type));


        if (value.isEmpty() && type == ldapcore::AttrType::Boolean)
        {
            value = "FALSE";
            QRegExp rx("TRUE|FALSE$");
            QRegExpValidator v(rx, 0);
            ui->valueEdit->setValidator(&v);
        }
        if (value.isEmpty() && type == ldapcore::AttrType::Integer)
        {
            value = "0";
            QRegExp rx("^\\d+$");
            QRegExpValidator v(rx, 0);
            ui->valueEdit->setValidator(&v);
        }
        if (value.isEmpty() && type == ldapcore::AttrType::GeneralizedTime)
        {
            value = QDateTime::currentDateTime().toString("yyyyMMddHHmmss.zzz") + "Z";
            QRegExp rx("^\\d{4}\\d{2}\\d{2}([0-9]|0[0-9]|1[0-9]|2[0-3])[0-5][0-9][0-5][0-9](\\.\\d{3})?$");
            QRegExpValidator v(rx, 0);
            ui->valueEdit->setValidator(&v);
        }
        ui->valueEdit->setText(value);
    }
}

ldapcore::CLdapAttribute CLdapNewAttributeDialog::attribute()
{
    ldapcore::CLdapAttribute attr;
    attr.setEditState(ldapcore::AttributeState::AttributeReadWrite);

    int clsIdx = ui->classCombo->currentIndex();
    int attrIdx = ui->attrCombo->itemData(ui->attrCombo->currentIndex()).toInt();

    QStringList classes{ui->classCombo->itemText(clsIdx)};
    attr.setClasses(classes);
    auto name = m_attributes[attrIdx].name();
    attr.setName(name);
    auto type = m_attributes[attrIdx].type();
    attr.setType(type);
    auto value = ui->valueEdit->text();
    attr.setValue(value);

    attr.setEditState(ldapcore::AttributeState::AttributeValueReadWrite);

    return attr;
}

}//namespace ldapeditor
