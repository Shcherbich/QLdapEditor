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
{
    m_contextMenu.addAction(m_newEntry);
    m_contextMenu.addAction(m_editEntry);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeView::customContextMenuRequested, this, &CLdapTreeView::customContextMenuRequested);
    connect(m_newEntry, &QAction::triggered, this, &CLdapTreeView::onNewEntry);
    connect(m_editEntry, &QAction::triggered, this, &CLdapTreeView::onEditEntry);

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

void CLdapTreeView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    emit treeItemChanged(current, previous);
}

void CLdapTreeView::onNewEntry()
{
    using namespace ldapcore;

    CLdapTreeProxyModel* proxyModel = static_cast<CLdapTreeProxyModel*>(model());
    QModelIndex curIndex = proxyModel->mapToSource(currentIndex());
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
    QString rdn = dialog.rdn();
    std::string delim = "=";
    auto v = split(rdn.toStdString(), delim);

    std::map<std::string, std::string> a2v;
    if (v.size() > 1)
    {
        a2v[v[0]] = v[1];
    }
    QVector<QString> classes = dialog.selectedClasses();
    QVector<ldapcore::CLdapAttribute> all_attributes = m_LdapData.schema().attributeByClasses(classes, a2v);
    QVector<ldapcore::CLdapAttribute> must_attributes ;
    for(ldapcore::CLdapAttribute& a: all_attributes)
    {
        if(!a.isMust())
            continue;

        QVector<QString> attrClasses;
        attrClasses = m_LdapData.schema().classesByAttributeName(a.name().toStdString(), classes);
        a.setClasses(attrClasses);
        must_attributes.push_back(a);
    }


    QModelIndex addIndex = static_cast<CLdapTreeModel*>(proxyModel->sourceModel())->addNewEntry(curIndex, rdn, dn, classes, must_attributes);
    setCurrentIndex(proxyModel->mapFromSource(addIndex));
    setExpanded(curIndex, true);
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
        return QString::compare(a.name(), "objectClass", Qt::CaseInsensitive);
    });
    auto fStructuralObjectClass = std::find_if(reallyAttributes.begin(), reallyAttributes.end(), [&](const ldapcore::CLdapAttribute& a)
    {
        return QString::compare(a.name(), "structuralObjectClass", Qt::CaseInsensitive);
    });

    if (fObjectClass == reallyAttributes.end())
    {
        QMessageBox::critical(this, tr("Error"), tr("No found attribut 'objectClass'"), QMessageBox::Ok);
        return;
    }
    if (fStructuralObjectClass == reallyAttributes.end())
    {
        QMessageBox::critical(this, tr("Error"), tr("No found attribut 'structuralObjectClass'"),QMessageBox::Ok);
        return;
    }

    auto structuralObjectClass = fStructuralObjectClass->value().toStdString();
    std::string delim = ";";
    auto prevClasses = split(fObjectClass->value().toStdString(), delim);
    prevClasses.erase(std::remove(prevClasses.begin(), prevClasses.end(), structuralObjectClass), prevClasses.end());
    auto dn = thisEntry->dn();
    auto rdn = thisEntry->rDn();
    ldapeditor::CLdapNewEntryDialog dialog(nullptr, dn, rdn, structuralObjectClass, prevClasses, m_LdapData);
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }
    QVector<ldapcore::CLdapAttribute> theseAttributes;
    thisEntry->loadAttributes(theseAttributes, false);
    auto fDelete = std::find_if(theseAttributes.begin(), theseAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
        return strcasecmp(a.name().toStdString().c_str(), "objectClass") == 0;
    });
    if (theseAttributes.end() != fDelete)
        theseAttributes.remove(std::distance(theseAttributes.begin(), fDelete));
    auto classes = dialog.selectedClasses();
    QVector<QString> tmp;
    for (auto& c : split(fObjectClass->value().toStdString(), delim))
        tmp << c.c_str();

    std::map<std::string, std::string> a2v;
    auto prevAttributes = m_LdapData.schema().attributeByClasses(tmp, a2v);
    auto currAttributes = m_LdapData.schema().attributeByClasses(classes, a2v);

    // delete attributes, which are not needed now
    for (auto& thisA : theseAttributes)
    {
        auto bPrev = std::find_if(prevAttributes.begin(), prevAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
            return strcasecmp(a.name().toStdString().c_str(), thisA.name().toStdString().c_str()) == 0;
        }) != prevAttributes.end();
        auto bCurr = std::find_if(currAttributes.begin(), currAttributes.end(), [&](const ldapcore::CLdapAttribute& a) {
            return strcasecmp(a.name().toStdString().c_str(), thisA.name().toStdString().c_str()) == 0;
        }) != currAttributes.end();

        if (!bCurr)
        {
            QMetaObject::invokeMethod(model(), "removeAttribute", Qt::ConnectionType::QueuedConnection, Q_ARG(QString, thisA.name()));
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
            return strcasecmp(a.name().toStdString().c_str(), currA.name().toStdString().c_str()) == 0;
        }) != theseAttributes.end();

        if (!bThese) {
            QMetaObject::invokeMethod(model(), "addAttribute", Qt::ConnectionType::QueuedConnection, Q_ARG(QString, currA.name()));
            continue;
        }
    }

    thisEntry->setClasses(classes);
    thisEntry->setEditable(true);
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

} //namespace ldapeditor
