#include <tuple>
#include <functional>
#include "ldaptreeview.h"
#include "ldapeditordefines.h"
#include "ldapnewentrydialog.h"
#include "CLdapEntry.h"
#include "CLdapData.h"

extern std::vector<std::string> split(const std::string& str, const std::string& delim);

namespace ldapeditor
{
CLdapTreeView::CLdapTreeView(QWidget* parent, ldapcore::CLdapData& data)
	: QTreeView(parent), m_LdapData(data)
	, m_newEntry(new QAction("New entry", this))
{
	m_contextMenu.addAction(m_newEntry);
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QTreeView::customContextMenuRequested, this, &CLdapTreeView::customContextMenuRequested);
	connect(m_newEntry, &QAction::triggered, this, &CLdapTreeView::onNewEntry);

}
void CLdapTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	emit treeItemChanged(current, previous);
}

void CLdapTreeView::onNewEntry()
{
	using namespace ldapcore;

	auto index = currentIndex();
	if (!index.isValid())
	{
		return;
	}

	CLdapEntry* currentEntry = static_cast<CLdapEntry*>(index.internalPointer());
	if (!currentEntry)
	{
		return;
	}

	auto dn = currentEntry->dn();
	ldapeditor::CLdapNewEntryDialog dialog(nullptr, dn, m_LdapData);
	if (dialog.exec() == QDialog::Rejected)
	{
		return;
	}
	auto rdn = dialog.rdn();
	std::string delim = "=";
	auto v = split(rdn.toStdString(), delim);

	std::map<std::string, std::string> a2v;
	if (v.size() > 1)
	{
		a2v[v[0]] = v[1];
	}

	auto classes = dialog.selectedClasses();
	auto attributes = m_LdapData.schema().attributeByClasses(classes, a2v);

	CLdapEntry* addEntry = new CLdapEntry(currentEntry, rdn, dn, classes, nullptr);
	addEntry->addAttributes(attributes);
	currentEntry->addNewChild(addEntry);
	model()->dataChanged(index, index);
	setExpanded(index, true);

	QModelIndexList Items = model()->match(index, Qt::DisplayRole, QVariant::fromValue(rdn), 2, Qt::MatchRecursive);
	if (Items.count() == 0)
	{
		return;
	}

	QModelIndex addIndex = Items.first();
	setCurrentIndex(addIndex);

}

void CLdapTreeView::customContextMenuRequested(QPoint pos)
{
	QModelIndex index = indexAt(pos);
	m_contextMenu.popup(viewport()->mapToGlobal(pos));
}

std::tuple<QModelIndex, ldapcore::CLdapEntry*> CLdapTreeView::findByDn(QString dn)
{
	QTreeView& tv(*this);
	QModelIndex modelIndex = tv.indexAt(tv.rect().topLeft());
	while (modelIndex.isValid())
	{
		modelIndex = tv.indexBelow(modelIndex);
		if (!modelIndex.isValid())
		{
			continue;
		}
		ldapcore::CLdapEntry* entry = static_cast<ldapcore::CLdapEntry*>(modelIndex.internalPointer());
		if (entry == nullptr)
		{
			continue;
		}
		if (dn == entry->dn())
		{
			return std::make_tuple(modelIndex, entry);
		}
	}
	return std::make_tuple(QModelIndex(), nullptr);
}

}//namespace ldapeditor


