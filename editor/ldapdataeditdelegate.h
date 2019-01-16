#ifndef LDAPDATAEDITDELEGATE_H
#define LDAPDATAEDITDELEGATE_H

#include <QStyledItemDelegate>
#include "CLdapAttribute.h"

namespace ldapcore{
class CLdapEntry;
}

namespace ldapeditor
{
class CLdapDataEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CLdapDataEditDelegate(QObject *parent = nullptr);
    void setLdapEntry(ldapcore::CLdapEntry* entry);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:

public slots:
protected:
    mutable ldapcore::AttrType m_attrType;
    mutable ldapcore::CLdapEntry* m_entry{nullptr};
};

} //namespace ldapeditor

#endif // LDAPDATAEDITDELEGATE_H
