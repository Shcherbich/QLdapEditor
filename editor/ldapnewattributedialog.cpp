#include "ldapnewattributedialog.h"
#include "ui_ldapnewattributedialog.h"
#include "attributemodelhelper.h"

namespace ldapeditor {

CLdapNewAttributeDialog::CLdapNewAttributeDialog(ldapcore::CLdapData &ldapData, ldapcore::CLdapEntry *entry, QWidget *parent) :
    QDialog(parent), m_LdapData(ldapData), m_entry(entry),
    ui(new Ui::CLdapNewAttributeDialog)
{
    ui->setupUi(this);

    connect(ui->classCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentClassChanged(int)));
    connect(ui->attrCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentAttributeChanged(int)));

    QStringList itemsList;
    for(auto s : m_entry->classes())
        itemsList << s;

    itemsList.sort();

    for(auto s : itemsList)
        ui->classCombo->addItem(s);

    QVector<ldapcore::CLdapAttribute>* realAttributes = m_entry->attributes();
    QVector<ldapcore::CLdapAttribute>  mayAttributes = m_entry->availableAttributesMay();
    QVector<ldapcore::CLdapAttribute>  mustAttributes = m_entry->availableAttributesMust();

    for(auto ma : mustAttributes)
    {
        if (std::find_if(std::begin(*realAttributes), std::end(*realAttributes), [&](const ldapcore::CLdapAttribute& ra) {
            return ma.name().compare( ra.name(),Qt::CaseInsensitive) == 0;
        }) == std::end(*realAttributes))
        {
            m_addMust.insert(ma.name());
        }
    }

    for(auto ma : mayAttributes)
    {
        if (std::find_if(std::begin(*realAttributes), std::end(*realAttributes), [&](const ldapcore::CLdapAttribute& ra) {
            return ma.name().compare( ra.name(),Qt::CaseInsensitive) == 0;
        }) == std::end(*realAttributes))
        {
            m_addMay.insert(ma.name());
        }
    }

    ui->classCombo->setCurrentIndex(0);
    onCurrentClassChanged(0);
}

CLdapNewAttributeDialog::~CLdapNewAttributeDialog()
{
    delete ui;
}

void CLdapNewAttributeDialog::onCurrentClassChanged(int index)
{
    ui->attrCombo->setEnabled(index != -1);
    if(index != -1)
    {
        std::map<std::string, std::string> a2v;
        QVector<QString>classes{ui->classCombo->currentText()};
        m_attributes = m_LdapData.schema().attributeByClasses(classes, a2v);
        ui->attrCombo->clear();


        qSort(m_attributes.begin(), m_attributes.end(),[](const ldapcore::CLdapAttribute& a1, const ldapcore::CLdapAttribute& a2){
            return a1.name().toLower() < a2.name().toLower();
        });

        for(int i=0; i< m_attributes.size(); i++)
        {
            if(m_addMust.contains(m_attributes[i].name()))
            {
                ui->attrCombo->addItem(m_attributes[i].name(), i);
            }
            else if(m_addMay.contains(m_attributes[i].name()))
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
    ui->typeEdit->setEnabled(index != -1);
    ui->valueEdit->setEnabled(index != -1);

    if(index != -1)
    {
        CAttributeModelHelper helper(m_LdapData);
        ui->typeEdit->setText(helper.attributeType2String(m_attributes[index].type()));
        ui->valueEdit->setText(m_attributes[index].value());
    }
}

ldapcore::CLdapAttribute CLdapNewAttributeDialog::attribute()
{
    ldapcore::CLdapAttribute attr;
    attr.setEditState(ldapcore::AttributeState::AttributeReadWrite);

    int clsIdx = ui->classCombo->currentIndex();
    int attrIdx = ui->attrCombo->itemData(ui->attrCombo->currentIndex()).toInt();

    QVector<QString>classes{ui->classCombo->itemText(clsIdx)};
    attr.setClasses(classes);
    auto name = m_attributes[attrIdx].name();
    attr.setName(name);
    auto type = m_attributes[attrIdx].type();
    attr.setType(type);
    auto value = ui->valueEdit->text();
    attr.setValue(value);

    return attr;
}

}//namespace ldapeditor
