/*!
\file
\brief Implementation file for LDAP tree view

File contains  implementation for LDAP tree view
*/
#include "ldaptreeview.h"
#include "ldaptreemodel.h"

#include "CLdapData.h"
#include "CLdapEntry.h"
#include "ldapeditordefines.h"
#include "ldapnewentrydialog.h"
#include <QMessageBox>
#include <functional>
#include <tuple>

extern std::vector<std::string> split(const std::string& str, const std::string& delim);

namespace ldapeditor
{

CLdapTreeView::CLdapTreeView(QWidget* parent, ldapcore::CLdapData& ldapData)
    : QTreeView(parent)
    , m_LdapData(ldapData)
    , m_newEntry(new QAction(tr("New entry"), this))
    , m_editEntry(new QAction(tr("Edit entry"), this))
    , m_deleteEntry(new QAction(tr("Delete entry"), this))
{
    m_contextMenu.addAction(m_newEntry);
    m_contextMenu.addAction(m_editEntry);
    m_contextMenu.addAction(m_deleteEntry);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTreeView::customContextMenuRequested, this, &CLdapTreeView::customContextMenuRequested);
    connect(m_newEntry, &QAction::triggered, this, &CLdapTreeView::onNewEntry);
    connect(m_editEntry, &QAction::triggered, this, &CLdapTreeView::onEditEntry);
    connect(m_deleteEntry, &QAction::triggered, this, &CLdapTreeView::onDeleteEntry);

    connect(this, &QTreeView::expanded, this, &CLdapTreeView::expand );

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

void CLdapTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    emit treeItemChanged(current, previous);
    QTreeView::currentChanged(current,previous);
}


void CLdapTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    CLdapTreeProxyModel* proxyModel = static_cast<CLdapTreeProxyModel*>(model());
    QModelIndex thisIndex = proxyModel->mapToSource(index);
    ldapcore::CLdapEntry* thisEntry = static_cast<ldapcore::CLdapEntry*>(thisIndex.internalPointer());
    if (thisEntry)
    {
        if(thisEntry->isLoaded() && thisEntry->children().isEmpty())
            return;
    }
    QTreeView::drawBranches(painter, rect, index);
}

void CLdapTreeView::onNewEntry()
{
    using namespace ldapcore;

    CLdapTreeProxyModel* proxyModel = static_cast<CLdapTreeProxyModel*>(model());
    QModelIndex currentIdx = currentIndex();
    QModelIndex curIndex = proxyModel->mapToSource(currentIdx);
    if (!curIndex.isValid())
    {
        return;
    }

    QString dn = curIndex.data(LdapTreeRoles::TreeDnRole).toString();
    ldapeditor::CLdapNewEntryDialog dialog(nullptr, dn, m_LdapData);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }


    QVector<QString> classes = m_LdapData.schema().consistentClassesByStructuralAndOther(dialog.structuralClass(), dialog.selectedClasses());

    QString rdn = dialog.rdn();
    std::string delim = "=";
    auto v = split(rdn.toStdString(), delim);
    std::map<std::string, std::string> a2v;
    QStringList list;
    for (auto cl: classes)
        list << cl;
    a2v["objectClass"] = list.join(";").toStdString();
    if (v.size() > 1)
    {
        a2v[v[0]] = v[1];
    }

    QVector<ldapcore::CLdapAttribute> all_attributes = m_LdapData.schema().attributeByClasses(classes, a2v);
    QVector<ldapcore::CLdapAttribute> newEntryAttributes ;
    for(ldapcore::CLdapAttribute& a: all_attributes)
    {
        if(!a.isMust() && a.value().isEmpty())
            continue;

        a.setIsIgnore(a.isMust() && a.value().isEmpty());
        QVector<QString> attrClasses;
        attrClasses = m_LdapData.schema().classesByAttributeName(a.name().toStdString(), classes);
        a.setClasses(attrClasses);
        newEntryAttributes.push_back(a);
    }


    QModelIndex addIndex = static_cast<CLdapTreeModel*>(proxyModel->sourceModel())->addNewEntry(curIndex, rdn, dn, classes, newEntryAttributes);
    setExpanded(currentIdx, true);
    setCurrentIndex(proxyModel->mapFromSource(addIndex));
    scrollTo(proxyModel->mapFromSource(addIndex), QAbstractItemView::PositionAtCenter);
}

void CLdapTreeView::onEditEntry()
{
    using namespace ldapcore;
    auto index = static_cast<CLdapTreeProxyModel*>(model())->mapToSource(currentIndex());
    if (!index.isValid())
    {
        return;
    }

    CLdapEntry* thisEntry = static_cast<CLdapEntry*>(index.internalPointer());
    if (!thisEntry)
    {
        return;
    }

    QVector<ldapcore::CLdapAttribute> reallyAttributes;
    thisEntry->loadAttributes(reallyAttributes);

    auto fObjectClass = std::find_if(reallyAttributes.begin(), reallyAttributes.end(), [&](const ldapcore::CLdapAttribute& a)
    {
        return a.name().compare("objectClass", Qt::CaseInsensitive) == 0;
    });

    QString fStructuralObjectClass = thisEntry->structuralClass();

    if (fObjectClass == reallyAttributes.end())
    {
        QMessageBox::critical(this, tr("Edit Entry Error"), tr("No found attribut 'objectClass'"), QMessageBox::Ok);
        return;
    }
    if (fStructuralObjectClass.isEmpty())
    {
        QMessageBox::critical(this, tr("Edit Entry Error"), tr("'structuralObjectClass' is not found"),QMessageBox::Ok);
        return;
    }

    auto structuralObjectClass = fStructuralObjectClass.toStdString();
    std::string delim = ";";

    QVector<QString> originalClasses = thisEntry->classes();
    QVector<QString> auxEntryClasses = thisEntry->auxiliaryClasses();
    QVector<QString> structClasses            ;
    std::vector<std::string> auxClasses;
    for(QString c: auxEntryClasses)
    {
         auxClasses.push_back(c.toStdString());
    }

    // fill struct classes
    std::for_each(originalClasses.constBegin(), originalClasses.constEnd(), [&structClasses,&auxClasses](const QString& c){
      if(std::find(auxClasses.begin(),auxClasses.end(),c.toStdString()) == auxClasses.end())
          structClasses.append(c);
    });

    QString dn = thisEntry->dn();
    QString rdn = thisEntry->rDn();
    ldapeditor::CLdapNewEntryDialog dialog(nullptr, dn, rdn, structuralObjectClass, auxClasses, m_LdapData);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }
    QVector<ldapcore::CLdapAttribute> theseAttributes;
    thisEntry->loadAttributes(theseAttributes, false);

    auto fDelete = std::find_if(theseAttributes.begin(), theseAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
        return a.name().compare("objectClass", Qt::CaseInsensitive) == 0;
    });

    if (theseAttributes.end() != fDelete)
        theseAttributes.remove(std::distance(theseAttributes.begin(), fDelete));

    QVector<QString> newClasses = structClasses ;
    newClasses << dialog.selectedClasses();


    std::map<std::string, std::string> a2v;
    auto prevAttributes = m_LdapData.schema().attributeByClasses(originalClasses, a2v);
    auto currAttributes = m_LdapData.schema().attributeByClasses(newClasses, a2v);

    // delete attributes, which are not needed now
    for (auto& thisA : theseAttributes)
    {
        auto bPrev = std::find_if(prevAttributes.begin(), prevAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
            return a.name().compare(thisA.name(), Qt::CaseInsensitive) == 0;
        }) != prevAttributes.end();

        auto bCurr = std::find_if(currAttributes.begin(), currAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
            return a.name().compare(thisA.name(), Qt::CaseInsensitive) == 0;
        }) != currAttributes.end();

        if (!bCurr)
        {
            emit onRemoveAttribute(thisA);
            continue;
        }

        if (bCurr && !bPrev)
        {
            continue;
        }
    }

    // add attributes, which are needed now
    for (auto& currA : currAttributes)
    {

        auto bThese = std::find_if(theseAttributes.begin(), theseAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
            return a.name().compare(a.name(), Qt::CaseInsensitive) == 0;
        }) != theseAttributes.end();

        if (!bThese)
        {
            emit onAddAttribute(currA);
            continue;
        }
    }

    thisEntry->setClasses(newClasses);
    thisEntry->setEditable(true);
}

void CLdapTreeView::onDeleteEntry()
{
    auto index = currentIndex();
    auto indexSource = static_cast<CLdapTreeProxyModel*>(model())->mapToSource(index);
    if (!index.isValid() || !indexSource.isValid())
    {
        return;
    }

    ldapcore::CLdapEntry* thisEntry = static_cast<ldapcore::CLdapEntry*>(indexSource.internalPointer());
    if (!thisEntry)
    {
        return;
    }

    auto ret = QMessageBox::question(this, tr("Question"), QString(tr("Do you really want to delete '%1' entry?").arg(thisEntry->dn())), QMessageBox::Yes|QMessageBox::No);
    if (ret != QMessageBox::Yes)
    {
        return;
    }

    try
    {
        m_LdapData.server().del(*thisEntry);
    }
    catch (const std::exception& ex)
    {
        QMessageBox::critical(this, tr("Delete Entry Error"), ex.what(), QMessageBox::Ok);
        return;
    }

    QModelIndex parent = index.parent();
    if(model()->removeRows(index.row(), 1, parent))
    {
        setCurrentIndex(parent);
        scrollTo(parent, QAbstractItemView::PositionAtCenter);
    }
}

void  CLdapTreeView::expand(const QModelIndex &index)
{
    if (!index.isValid())
    {
        return;
    }

    auto srcIndex = static_cast<CLdapTreeProxyModel*>(model())->mapToSource(index);
    if (!srcIndex.isValid())
    {
        return;
    }

    ldapcore::CLdapEntry* thisEntry = static_cast<ldapcore::CLdapEntry*>(srcIndex.internalPointer());
    if (!thisEntry)
    {
        return;
    }

    if(!thisEntry->isLoaded())
    {
        CLdapTreeProxyModel* proxyModel = static_cast<CLdapTreeProxyModel*>(model());
        auto size = thisEntry->children().size();
        if (size != 0)
        {
            static_cast<CLdapTreeModel*>(proxyModel->sourceModel())->insertRows(0, size, srcIndex);
            update(index);
        }
    }

    QTreeView::expand(index);
}

void CLdapTreeView::customContextMenuRequested(QPoint pos)
{
    m_contextMenu.popup(viewport()->mapToGlobal(pos));
}

std::tuple<QModelIndex, ldapcore::CLdapEntry*> CLdapTreeView::findByDn(QString dn)
{
    QTreeView& tv(*this);
   // QModelIndex modelIndex = tv.indexAt(tv.rect().topLeft());
    QModelIndex modelIndex = static_cast<CLdapTreeProxyModel*>(model())->mapToSource(tv.indexAt(tv.rect().topLeft()));
    while (modelIndex.isValid())
    {
        modelIndex = tv.indexBelow(modelIndex);
        if (!modelIndex.isValid())
        {
            continue;
        }
        ldapcore::CLdapEntry* entry = static_cast<ldapcore::CLdapEntry*>(modelIndex.internalPointer());
        if (!entry)
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


/*
void CLdapTreeView::saveState()
{
    m_itemsList.clear();
    foreach(QModelIndex index, static_cast<CLdapTreeProxyModel*>(model())->getPersistentIndexList())
    {
        if(isExpanded(index))
            m_itemsList << index.data(Qt::DisplayRole).toString();
    }
    m_currentItem = currentIndex().data(Qt::DisplayRole).toString();
}
void CLdapTreeView::restoreState()
{
    QModelIndex startIdx = model()->index(0,0);
    foreach(QString item, m_itemsList)
    {
        QModelIndexList items = model()->match(startIdx, Qt::DisplayRole, QVariant::fromValue(item));
        if(!items.isEmpty())
        {
            startIdx = items.first();
            setExpanded(startIdx, true);
        }
    }

    QModelIndexList items = model()->match(startIdx, Qt::DisplayRole, QVariant::fromValue(m_currentItem));
    if(!items.isEmpty())
        setCurrentIndex(items.first());
}
*/
} //namespace ldapeditor
