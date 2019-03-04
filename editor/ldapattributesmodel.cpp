/*!
@file
@brief |Implementation file for LDAP Entity's attributes  model class

File contains  implementations for LDAP Entity's attributes model class
*/

#include "ldapattributesmodel.h"
#include "ldapeditordefines.h"
#include <QApplication>
#include <QDate>
#include <QMessageBox>
#include <QTime>
#include <QtDebug>

namespace ldapeditor
{

bool CLdapAttributesProxyModel::filterAcceptsRow(int sourceRow,  const QModelIndex &sourceParent) const
{
  QModelIndex indexAttr = sourceModel()->index(sourceRow, static_cast<int>(ldapeditor::AttributeColumn::Attribute), sourceParent);
  QModelIndex indexValue = sourceModel()->index(sourceRow, static_cast<int>(ldapeditor::AttributeColumn::Value), sourceParent);

  if(!filterRegExp().isEmpty())
        return sourceModel()->data(indexAttr).toString().contains(filterRegExp()) ||
               sourceModel()->data(indexValue).toString().contains(filterRegExp()) ;

  return true;
}


CLdapAttributesModel::CLdapAttributesModel(ldapcore::CLdapData& ldapData, QObject* parent)
    : QAbstractTableModel(parent)
    , m_LdapData(ldapData)
    , m_SectionsList { tr("Ignore"), tr("Name"), tr("Class"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
    , m_attrHelper(m_LdapData)
{
}

void CLdapAttributesModel::setLdapEntry(ldapcore::CLdapEntry* entry)
{
    beginResetModel();
    int iRowCount = rowCount();
    if (iRowCount != 0)
    {
        removeRows(0, iRowCount);
    }
    if (m_pAttributes)
    {
        m_pAttributes->clear();
    }
    if (m_entry && entry != m_entry)
    {
        m_entry->setEditable(false);
    }
    m_entry = entry;
    m_attrHelper.setLdapEntry(m_entry);
    m_pAttributes = m_entry ? m_entry->attributes() : nullptr;

    endResetModel();

    if (m_pAttributes)
    {
        emit dataChanged(index(0, 0), index(m_pAttributes->size(), m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
    }
    m_IsChanged = false;
}

QVariant CLdapAttributesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0)
    {
        return QVariant();
    }
    if (section >= m_SectionsList.size())
    {
        return QVariant();
    }
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    return m_SectionsList[section];
}

bool CLdapAttributesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (value != headerData(section, orientation, role))
    {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int CLdapAttributesModel::rowCount(const QModelIndex& parent) const
{
    return m_pAttributes ? m_pAttributes->count() : 0;
}

int CLdapAttributesModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    // FIXME: Implement me!
    return m_SectionsList.size();
}

QVariant CLdapAttributesModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role);
    if (!m_pAttributes || !index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= m_pAttributes->size())
    {
        return QVariant();
    }

    // FIXME: Implement me!
    const ldapcore::CLdapAttribute& attr = (*m_pAttributes)[index.row()];
    return m_attrHelper.data(attr, index, role);
}

bool CLdapAttributesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!m_pAttributes || !index.isValid())
    {
        return false;
    }

    ldapcore::CLdapAttribute& attr = (*m_pAttributes)[index.row()];
    if (role == Qt::EditRole)
    {
        if (data(index, role) != value)
        {
            bool bRet { true };
            try
            {
                if (index.column() == static_cast<int>(AttributeColumn::Value)) // changing value
                {
                    ldapcore::CLdapAttribute temp(attr);
                    if(attr.type() == ldapcore::AttrType::Boolean)
                        temp.setValue(value.toBool()?"TRUE" : "FALSE");
                    else
                        temp.setValue(value.toString());
                    m_entry->validateAttribute(temp);
                }
                bRet = m_attrHelper.setData(attr, index, value, role);
                emit dataChanged(index, index, QVector<int>() << role << Qt::DisplayRole);
            }
            catch (const std::exception& e)
            {
                QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
                bRet = false;
            }
            return bRet;
        }
    }
    if (role == Qt::CheckStateRole)
    {
        bool bRet { true };
        if (data(index, role) != value)
        {
            if (index.column() == static_cast<int>(AttributeColumn::Ignore)) // changing value
            {
                bRet = m_attrHelper.setData(attr, index, value, role);
                emit dataChanged(index, index, QVector<int>() << role  << Qt::CheckStateRole);
            }
            else
            {
                bRet = false;
            }
        }
        return bRet;
    }
    return false;
}

Qt::ItemFlags CLdapAttributesModel::flags(const QModelIndex& index) const
{
    if (!m_pAttributes || !index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags checkableFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable};
    Qt::ItemFlags readonlyFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
    Qt::ItemFlags editFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable };

    if (index.row() < m_pAttributes->size())
    {
        int col = index.column();
        ldapcore::AttributeState editState = (*m_pAttributes)[index.row()].editState();

        bool checkableItem = col == static_cast<int>(AttributeColumn::Ignore) &&
                             m_entry->isNew() && (*m_pAttributes)[index.row()].isMust() ;

        if (checkableItem)
        {
            return checkableFlags;
        }

        switch (editState)
        {
        case ldapcore::AttributeState::AttributeReadOnly:
            return readonlyFlags;
            break;
        case ldapcore::AttributeState::AttributeReadWrite:
            return (col == static_cast<int>(AttributeColumn::Attribute) || col == static_cast<int>(AttributeColumn::Value))
                   ? editFlags
                   : readonlyFlags;
            break;
        case ldapcore::AttributeState::AttributeValueReadWrite:
            if (checkableItem && (*m_pAttributes)[index.row()].isIgnore())
            {
                return readonlyFlags;
            }
            else
            {
                return col == static_cast<int>(AttributeColumn::Value) ? editFlags : readonlyFlags;
            }
            break;
        default:
            break;
        }
    }
    return readonlyFlags;
}

bool CLdapAttributesModel::insertRows(int row, int count, const QModelIndex& parent)
{
    if (!m_pAttributes || !m_entry)
    {
        return false;
    }

    beginInsertRows(parent, row, row + count - 1);
    auto aNew = m_entry->createEmptyAttribute("name");
    aNew->setValue("");
    m_pAttributes->append(*aNew);
    endInsertRows();
    m_IsChanged = true;

    QModelIndex idxFrom = index(row, 0, parent);
    QModelIndex idxTo = index(row + count - 1, m_SectionsList.size() - 1, parent);
    emit dataChanged(idxFrom, idxTo, QVector<int>() << Qt::DisplayRole);
    return true;
}

QModelIndex CLdapAttributesModel::addAttribute(const ldapcore::CLdapAttribute& attribute)
{
    if (!m_pAttributes || !m_entry)
    {
        return QModelIndex();
    }

    auto aNew = m_entry->createEmptyAttribute(attribute.name().toStdString());
    aNew->setType(attribute.type());
    aNew->setValue(attribute.value());

    auto classes = attribute.classes();
    aNew->setClasses(classes);

    auto description = attribute.description();
    aNew->setDescription(description);

    aNew->setEditState(attribute.editState());

    int row = rowCount();
    int count = 1;
    QModelIndex parent;
    beginInsertRows(QModelIndex(), row, row + count - 1);
    m_pAttributes->append(*aNew);
    endInsertRows();

    QModelIndex idxFrom = index(row, 0, parent);
    QModelIndex idxTo = index(row + count - 1, m_SectionsList.size() - 1, parent);
    emit dataChanged(idxFrom, idxTo, QVector<int>() << Qt::DisplayRole);
    return idxFrom;
}

bool CLdapAttributesModel::insertColumns(int column, int count, const QModelIndex& parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
    return false;
}

bool CLdapAttributesModel::removeRows(int row, int count, const QModelIndex& parent)
{
    if (!m_pAttributes)
    {
        return true;
    }

    beginRemoveRows(parent, row, row + count - 1);
    m_pAttributes->remove(row, count);
    endRemoveRows();
    m_IsChanged = true;
    return true;
}

void CLdapAttributesModel::changedRows(
    QVector<ldapcore::CLdapAttribute>& newRows,
    QVector<ldapcore::CLdapAttribute>& deleteRows,
    QMap<QString, QVector<ldapcore::CLdapAttribute>>& updateMap)
{
    if (!m_pAttributes || !m_entry)
    {
        return;
    }
    QVector<ldapcore::CLdapAttribute> reallyAttributes;
    m_entry->loadAttributes(reallyAttributes, false);

    // first - save new attributes
    for (auto& a : *m_pAttributes)
    {
        if (!a.isModified())
        {
            continue;
        }
        auto f = std::find_if(reallyAttributes.begin(), reallyAttributes.end(), [&](const ldapcore::CLdapAttribute & o)
        {
            return a.name() == o.name();
        });
        if (f == reallyAttributes.end())
        {
            newRows.push_back(a);
        }
    }

    // second - delete attributes
    for (auto& r : reallyAttributes)
    {
        auto f = std::find_if(m_pAttributes->begin(), m_pAttributes->end(), [&](const ldapcore::CLdapAttribute & o)
        {
            return r.name() == o.name();
        });

        if (f == m_pAttributes->end())
        {
            deleteRows.push_back(r);
        }
    }

    // second - update attributes
    for (auto& a : *m_pAttributes)
    {
        if (updateMap.contains(a.name()))
        {
            continue;
        }
        if (a.isModified())
        {
            for (auto& attr : *m_pAttributes)
            {
                if (attr.name() == a.name())
                {
                    updateMap[a.name()].push_back(attr);
                }
            }
        }
    }

    for (auto& a : reallyAttributes)
    {
        if (updateMap.contains(a.name()))
        {
            continue;
        }
        auto wasDeleted = std::find_if(m_pAttributes->begin(), m_pAttributes->end(), [&](const ldapcore::CLdapAttribute & o)
        {
            return a.name() == o.name() && a.value() == o.value();
        }) == m_pAttributes->end();
        if (wasDeleted)
        {
            for (auto& attr : *m_pAttributes)
            {
                if (attr.name() == a.name())
                {
                    updateMap[a.name()].push_back(attr);
                }
            }
        }
    }
}

bool CLdapAttributesModel::isNew() const
{
    return m_entry ? m_entry->isNew() : false;
}

bool CLdapAttributesModel::isEdit() const
{
    return m_entry ? m_entry->isEdit() : false;
}


QString CLdapAttributesModel::dn() const
{
    return m_entry && m_entry->parent() ? m_entry->parent()->dn() : "";
}

bool CLdapAttributesModel::Save()
{
    bool saved = false;
    if (isNew())
    {
        saved = SaveNewEntry();
        if (saved)
        {
            setLdapEntry(m_entry);
        }
        return saved;
    }

    if (isEdit())
    {
        saved = SaveUpdatedEntry();
        return saved;
    }
    saved = SaveAttributes();
    if (saved)
    {
        setLdapEntry(m_entry);
    }
    return saved;
}

bool CLdapAttributesModel::SaveNewEntry()
{
    if (!m_pAttributes || !m_entry)
    {
        return false;
    }

    // check
    for (auto& a : *m_pAttributes)
    {
        if (a.isMust() && (!a.isIgnore() && a.value().isEmpty()))
        {
            QMessageBox::critical(nullptr, tr("Error"), QString(tr("The must attribute '%1' is not filled or explicitly marked it as ignored!")).arg(a.name()), QMessageBox::Ok);
            return false;
        }
    }

    // save to database
    try
    {
        m_LdapData.server().add(*m_entry);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool CLdapAttributesModel::SaveUpdatedEntry()
{
    if (!m_pAttributes || !m_entry)
    {
        return false;
    }

    // check
    for (auto& a : *m_pAttributes)
    {
        if (a.isMust() && (!a.isIgnore() && a.value().isEmpty()))
        {
            QMessageBox::critical(nullptr, tr("Error"), QString(tr("The must attribute '%1' is not filled or explicitly marked it as ignored!")).arg(a.name()), QMessageBox::Ok);
            return false;
        }
    }

    try
    {
        m_LdapData.server().update(*m_entry);
        m_entry->setEditable(false);
        m_entry->flushAttributesCache();
        setLdapEntry(m_entry);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        return false;
    }
    return true;
}

bool CLdapAttributesModel::SaveAttributes()
{
    if (!m_pAttributes || !m_entry)
    {
        return false;
    }

    QVector<ldapcore::CLdapAttribute> newRows, deleteRows;
    QMap<QString, QVector<ldapcore::CLdapAttribute>> updateMap;
    changedRows(newRows, deleteRows, updateMap);
    if (newRows.empty() && deleteRows.empty() && updateMap.empty())
    {
        return true;
    }
    for (auto& n : newRows)
    {
        try
        {
            m_LdapData.server().addAttribute(*m_entry, n);
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        }
    }
    for (auto& d : deleteRows)
    {
        try
        {
            m_LdapData.server().delAttribute(*m_entry, d);
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        }
    }


    QMapIterator<QString, QVector<ldapcore::CLdapAttribute> > i(updateMap);
    while (i.hasNext())
    {
        i.next();
        try
        {
            m_LdapData.server().updateAttributes(*m_entry, i.key(), i.value());
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_entry->flushAttributesCache();
    QApplication::restoreOverrideCursor();
    setLdapEntry(m_entry);
    return true;
}

void CLdapAttributesModel::onRemoveAttribute(const ldapcore::CLdapAttribute& attr)
{
    if (!m_pAttributes || !m_entry)
    {
        return ;
    }

    for (int i = 0; i < rowCount(); ++i)
    {
        QModelIndex index = createIndex(i, static_cast<int>(ldapeditor::AttributeColumn::Attribute));
        auto displayText = index.data(Qt::DisplayRole);
        if (displayText == attr.name())
        {
            removeRows(i, 1);
            return;
        }
    }
}

void CLdapAttributesModel::onAddAttribute(const ldapcore::CLdapAttribute& attr)
{
    if (!m_pAttributes || !m_entry)
    {
        return ;
    }

    addAttribute(attr);
    /*
    int row = rowCount();
    insertRows(row, 1);
    QModelIndex index = createIndex(row, static_cast<int>(ldapeditor::AttributeColumn::Attribute));
    setData(index, attr, Qt::EditRole);
    sortData();
    */
}

void CLdapAttributesModel::sortData()
{
    if (!m_pAttributes || !m_entry)
    {
        return ;
    }

    m_entry->sortAttributes();
    emit dataChanged(index(0, 0), index(m_pAttributes->size(), m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
}


} //namespace ldapeditor
