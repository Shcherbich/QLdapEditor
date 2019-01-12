#ifndef LDAPDATAEDITDELEGATE_H
#define LDAPDATAEDITDELEGATE_H

#include <QStyledItemDelegate>
#include "CLdapAttribute.h"

namespace ldapeditor
{
class CLdapDataEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit CLdapDataEditDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:

public slots:
protected:
    mutable ldapcore::AttrType m_attrType;
};

} //namespace ldapeditor

#endif // LDAPDATAEDITDELEGATE_H
