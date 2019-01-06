#ifndef LDAPTREEVIEW_H
#define LDAPTREEVIEW_H


#include <QTreeView>
#include <QSortFilterProxyModel>
namespace ldapeditor
{

    /*
    class CTreeFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        CTreeFilterProxyModel(QObject *parent = nullptr):QSortFilterProxyModel(parent)
        {
            setDynamicSortFilter(true);
            setFilterKeyColumn(0);
        }

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    };
    */
    class CLdapTreeView : public QTreeView
    {
        Q_OBJECT
    public:
        CLdapTreeView(QWidget *parent = nullptr);
    signals:
        void treeItemChanged(const QModelIndex &current, const QModelIndex &previous);

    protected:
        virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

    };
} //namespace ldapeditor


#endif // LDAPTREEVIEW_H
