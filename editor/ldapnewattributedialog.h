#ifndef LDAPNEWATTRIBUTEDIALOG_H
#define LDAPNEWATTRIBUTEDIALOG_H

#include <QDialog>
#include "CLdapEntry.h"
#include "CLdapData.h"
#include "CLdapAttribute.h"
#include <QVector>
#include <QSet>

namespace Ui {
class CLdapNewAttributeDialog;
}

namespace ldapeditor
{

class CLdapNewAttributeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLdapNewAttributeDialog(ldapcore::CLdapData&  ldapData, ldapcore::CLdapEntry* entry, QWidget *parent = nullptr);
    ~CLdapNewAttributeDialog();

    ldapcore::CLdapAttribute attribute();
protected slots:
    void onCurrentClassChanged(int index);
    void onCurrentAttributeChanged(int index);
private:
    ldapcore::CLdapData&  m_LdapData;
    ldapcore::CLdapEntry* m_entry{nullptr};
    Ui::CLdapNewAttributeDialog *ui;
    QVector<ldapcore::CLdapAttribute> m_attributes;

    QSet<QString> m_addMust;
    QSet<QString> m_addMay;
};
} //namespace ldapeditor
#endif // LDAPNEWATTRIBUTEDIALOG_H
