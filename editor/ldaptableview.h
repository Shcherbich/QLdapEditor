#ifndef LDAPTABLEVIEW_H
#define LDAPTABLEVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
//#include <QSortFilterProxyModel>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include "ldapdataeditdelegate.h"
#include "CLdapEntry.h"
#include "ldapsettings.h"

namespace ldapeditor
{
    class CLdapTableView : public QTableView
    {
        Q_OBJECT
    public:
        explicit CLdapTableView(QWidget *parent, ldapcore::CLdapData&, CLdapSettings&);
        void setLdapEntry(ldapcore::CLdapEntry* entry);
        void RestoreView();
        bool SaveData();

    signals:

    public slots:
        void customContextMenuRequested(QPoint pos);
    protected slots:
        void onNewAttribute();
        void onDeleteAttribute();
    protected:
        virtual bool edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)override;
        void mousePressEvent(QMouseEvent *event)override;

        void OnHeaderChanged(int logicalIndex, int oldSize, int newSize);

        CLdapDataEditDelegate m_ldapDataDelegate;
        QStyledItemDelegate   m_defaultDelegate;
        ldapcore::CLdapData&  m_LdapData;
        CLdapSettings&        m_LdapSettings;
        QMenu m_contextMenu;
        QAction* m_newAttr{nullptr};
        QAction* m_delAttr{nullptr};
        ldapcore::CLdapEntry* m_entry{nullptr};
    };

} //namespace ldapeditor
#endif // LDAPTABLEVIEW_H
