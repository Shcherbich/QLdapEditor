#ifndef LDAPTREEVIEW_H
#define LDAPTREEVIEW_H


#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <tuple>
#include "CLdapEntry.h"

namespace ldapcore
{
class CLdapData;
}


namespace ldapeditor
{

class CLdapTreeView : public QTreeView
{
	Q_OBJECT
public:
	CLdapTreeView(QWidget* parent, ldapcore::CLdapData&);
	std::tuple<QModelIndex, ldapcore::CLdapEntry*> findByDn(QString dn);

signals:
    void treeItemChanged(const QModelIndex& current, const QModelIndex& previous);

protected :
	virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

protected slots:
	void customContextMenuRequested(QPoint pos);
	void onNewEntry();
    void onEditEntry();

private:
	QMenu       m_contextMenu;
	QAction*    m_newEntry{nullptr};
    QAction*    m_editEntry{nullptr};
	ldapcore::CLdapData& m_LdapData;

};
} //namespace ldapeditor


#endif // LDAPTREEVIEW_H
