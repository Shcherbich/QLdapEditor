#ifndef LDAPTABLEVIEW_H
#define LDAPTABLEVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include "ldapdataeditdelegate.h"

namespace ldapeditor
{
    class CLdapTableView : public QTableView
    {
        Q_OBJECT
    public:
        explicit CLdapTableView(QWidget *parent = nullptr);

    signals:

    public slots:

    protected:
        virtual bool edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)override;
        CLdapDataEditDelegate m_ldapDataDelegate;
        QStyledItemDelegate   m_defaultDelegate;
    };

} //namespace ldapeditor
#endif // LDAPTABLEVIEW_H
