#include "ldapattributesmodel.h"
#include "ldapeditordefines.h"
#include <QApplication>
#include <QDate>
#include <QMessageBox>
#include <QTime>
#include <QtDebug>

namespace ldapeditor {
CLdapAttributesModel::CLdapAttributesModel(ldapcore::CLdapData &ldapData, QObject* parent)
    : QAbstractTableModel(parent)
    , m_LdapData(ldapData)
    , m_SectionsList { tr("Name"), tr("Class"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
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
    m_attrHelper.setLdapEntry(entry);

    m_pAttributes = entry->attributes();
    endResetModel();
    emit dataChanged(index(0, 0), index(m_pAttributes->size(), m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
    m_IsChanged = false;
}

QVariant CLdapAttributesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 0)
        return QVariant();
    if (section >= m_SectionsList.size())
        return QVariant();
    if (orientation != Qt::Horizontal)
        return QVariant();
    if (role != Qt::DisplayRole)
        return QVariant();

    return m_SectionsList[section];
}

bool CLdapAttributesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
    if (value != headerData(section, orientation, role)) {
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
        return 0;

    // FIXME: Implement me!
    return m_SectionsList.size();
}

QVariant CLdapAttributesModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role);
    if (!index.isValid())
        return QVariant();
    if (index.row() >= m_pAttributes->size())
        return QVariant();

    // FIXME: Implement me!
    const ldapcore::CLdapAttribute& attr = (*m_pAttributes)[index.row()];
    return m_attrHelper.data(attr, index, role);
}

bool CLdapAttributesModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    ldapcore::CLdapAttribute& attr = (*m_pAttributes)[index.row()];
    if (role == Qt::EditRole) {
        if (data(index, role) != value) {
            bool bRet { true };
            try {
                if (index.column() == static_cast<int>(AttributeColumn::Value)) // changing value
                {
                    ldapcore::CLdapAttribute temp(attr);
                    temp.setValue(value.toString());
                    m_entry->validateAttribute(temp);
                }
                bRet = m_attrHelper.setData(attr, index, value, role);
                emit dataChanged(index, index, QVector<int>() << role << Qt::DisplayRole);
            } catch (const std::exception& e) {
                QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
                bRet = false;
            }
            return bRet;
        }
    }

    return false;
}

Qt::ItemFlags CLdapAttributesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags readonlyFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };
    Qt::ItemFlags editFlags { Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable };
    if (index.row() < m_pAttributes->size()) {
        int col = index.column();
        ldapcore::AttributeState editState = (*m_pAttributes)[index.row()].editState();
        switch (editState) {
        case ldapcore::AttributeState::AttributeReadOnly:
            return readonlyFlags;
            break;
        case ldapcore::AttributeState::AttributeReadWrite:
            return col == static_cast<int>(AttributeColumn::Attribute) || col == static_cast<int>(AttributeColumn::Value) ? editFlags : readonlyFlags;
            break;
        case ldapcore::AttributeState::AttributeValueReadWrite:
            return col == static_cast<int>(AttributeColumn::Value) ? editFlags : readonlyFlags;
            break;
        default:
            break;
        }
    }
    return readonlyFlags;
}

bool CLdapAttributesModel::insertRows(int row, int count, const QModelIndex& parent)
{
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
        return true;

    beginRemoveRows(parent, row, row + count - 1);
    m_pAttributes->remove(row, count);
    endRemoveRows();
    m_IsChanged = true;
    return true;
}

void CLdapAttributesModel::GetChangedRows(QVector<ldapcore::CLdapAttribute>& newRows,
    QVector<ldapcore::CLdapAttribute>& deleteRows,
    QVector<ldapcore::CLdapAttribute>& updateRows)
{
    if (m_pAttributes == nullptr) {
        return;
    }
    QVector<ldapcore::CLdapAttribute> reallyAttributes;
    m_entry->loadAttributes(reallyAttributes);

    // first - save new attributes
    for (auto& a : *m_pAttributes) {
        auto f = std::find_if(reallyAttributes.begin(), reallyAttributes.end(), [&](const ldapcore::CLdapAttribute& o) {
            return a.name() == o.name();
        });
        if (f == reallyAttributes.end() && a.isModified()) {
            newRows.push_back(a);
        }
    }

    // second - delete attributes
    for (auto& r : reallyAttributes) {
        auto f = std::find_if(m_pAttributes->begin(), m_pAttributes->end(), [&](const ldapcore::CLdapAttribute& o) {
            return r.name() == o.name();
        });
        if (f == m_pAttributes->end()) {
            deleteRows.push_back(r);
        }
    }

    // second - update attributes
    for (auto& r : reallyAttributes) {
        auto f = std::find_if(m_pAttributes->begin(), m_pAttributes->end(), [&](const ldapcore::CLdapAttribute& o) {
            return r.name() == o.name();
        });
        if (f != m_pAttributes->end() && f->isModified()) {
            updateRows.push_back(*f);
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
    if (isNew()) {
        return SaveNewEntry();
    }
    if (isEdit()) {
        return SaveUpdatedEntry();
    }
    return SaveAttributes();
}

bool CLdapAttributesModel::SaveNewEntry()
{
    // check
    for (auto& a : *m_pAttributes)
    {
        if (a.isMust() && a.value().isEmpty())
        {
            QMessageBox::critical(nullptr, tr("Error"), QString(tr("The must attribute '%1' is not filled!")).arg(a.name()), QMessageBox::Ok);
            return false;
        }
    }

    // save to database
    try
    {
        auto parent = m_entry->parent();
        if (parent != nullptr)
        {
            parent->saveNewChild();
        }
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
    // check
    for (auto& a : *m_pAttributes)
    {
        if (a.isMust() && a.value().isEmpty())
        {
            QMessageBox::critical(nullptr, tr("Error"), QString(tr("The must attribute '%1' is not filled!")).arg(a.name()), QMessageBox::Ok);
            return false;
        }
    }

    try
    {
        m_entry->update();
        m_entry->setEditable(false);
        m_entry->flushAttributeCache();
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
    QVector<ldapcore::CLdapAttribute> newRows, deleteRows, updateRows;
    GetChangedRows(newRows, deleteRows, updateRows);
    if (!newRows.size() && !deleteRows.size() && !updateRows.size())
    {
        return true;
    }
    for (auto& n : newRows)
    {
        try
        {
            m_entry->addAttribute(n);
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
            m_entry->deleteAttribute(d);
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        }
    }
    for (auto& u : updateRows)
    {
        try
        {
            m_entry->updateAttribute(u);
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(nullptr, tr("Error"), e.what(), QMessageBox::Ok);
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_entry->flushAttributeCache();
    QApplication::restoreOverrideCursor();
    setLdapEntry(m_entry);
    return true;
}

void CLdapAttributesModel::onRemovingAttribute(QString name)
{
    for (int i = 0; i < rowCount(); ++i)
    {
        auto index = createIndex(i, 1);
        auto displayText = data(index, Qt::DisplayRole);
        if (displayText == name)
        {
            removeRows(i, 1);
            return;
        }
    }
}

void CLdapAttributesModel::onAddAttribute(QString name)
{
    int row = rowCount();
    insertRows(row, 1);
    auto i = index(row, 1);
    setData(i, name, Qt::EditRole);
    sortData();
}

void CLdapAttributesModel::sortData()
{
    m_entry->sortAttributes();
    emit dataChanged(index(0, 0), index(m_pAttributes->size(), m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
}


} //namespace ldapeditor
