#ifndef LDAPTREEVIEW_H
#define LDAPTREEVIEW_H


#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenu>

namespace ldapeditor
{

    class CLdapTreeView : public QTreeView
    {
        Q_OBJECT
    public:
        CLdapTreeView(QWidget *parent = nullptr);
    signals:
        void treeItemChanged(const QModelIndex &current, const QModelIndex &previous);

    protected:
        virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
        void customContextMenuRequested(QPoint pos);
        void onNewEntry();

    private:
        QMenu       m_contextMenu;
        QAction*    m_newEntry{nullptr};

    };
} //namespace ldapeditor


#endif // LDAPTREEVIEW_H
