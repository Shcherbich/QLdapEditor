#include "ldapattributesmodel.h"
#include "ldapeditordefines.h"
#include <QDate>
#include <QTime>
#include <QtDebug>

namespace ldapeditor
{
    CLdapAttributesModel::CLdapAttributesModel(QObject *parent)
        : QAbstractTableModel(parent)
        , m_SectionsList{ tr("Name"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
    {
    }


    void CLdapAttributesModel::setLdapEntry(ldapcore::CLdapEntry* entry)
    {
        beginResetModel();
        if(m_pAttributes)
            m_pAttributes->clear();
        removeRows(0, rowCount());

        m_entry = entry;
        m_pAttributes = entry->attributes();
        endResetModel();
        emit dataChanged(index(0,0), index(m_pAttributes->size(),m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
        m_IsChanged = false;

    }

    QVariant CLdapAttributesModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(section < 0) return QVariant();
        if(section >= m_SectionsList.size()) return QVariant();
        if(orientation != Qt::Horizontal) return QVariant();
        if(role != Qt::DisplayRole) return QVariant();

        return m_SectionsList[section];
    }

    bool CLdapAttributesModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
    {
        if (value != headerData(section, orientation, role)) {
            // FIXME: Implement me!
            emit headerDataChanged(orientation, section, section);
            return true;
        }
        return false;
    }


    int CLdapAttributesModel::rowCount(const QModelIndex &parent) const
    {
        return m_pAttributes ? m_pAttributes->count() : 0;
    }

    int CLdapAttributesModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        // FIXME: Implement me!
        return m_SectionsList.size();
    }

    QVariant CLdapAttributesModel::data(const QModelIndex &index, int role) const
    {
        Q_UNUSED(role);
        if (!index.isValid())  return QVariant();
        if(index.row()>= m_pAttributes->size()) return QVariant();

        // FIXME: Implement me!
        const ldapcore::CLdapAttribute&  attr = (*m_pAttributes)[index.row()];
        return m_attrHelper.data(attr, index,role);
    }

    bool CLdapAttributesModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid())
            return false;

        ldapcore::CLdapAttribute&  attr = (*m_pAttributes)[index.row()];
        if(role == Qt::EditRole)
        {
            if (data(index, role) != value)
            {
                bool bRet{true};
                try
                {
                    bRet = m_attrHelper.setData(attr, index, value, role);
                    emit dataChanged(index, index, QVector<int>() << role << Qt::DisplayRole);
                }
                catch(std::exception& e)
                {
                    Q_UNUSED(e)
                    bRet = false;
                }
                return bRet;
            }
        }

        return false;
    }



    Qt::ItemFlags CLdapAttributesModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags readonlyFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable};
        Qt::ItemFlags editFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable};
        if (index.row() < m_pAttributes->size())
        {
            int col = index.column();
            ldapcore::AttributeState editState = (*m_pAttributes)[index.row()].editState();
            switch(editState)
            {
            case ldapcore::AttributeState::AttributeReadOnly:
                return readonlyFlags; break;
            case ldapcore::AttributeState::AttributeReadWrite:
                return col == 1 || col == 2 ? editFlags : readonlyFlags; break;
            case ldapcore::AttributeState::AttributeValueReadWrite:
                return col == 2 ? editFlags : readonlyFlags; break;
            default: break;
            }
        }
        return readonlyFlags;
    }

    bool CLdapAttributesModel::insertRows(int row, int count, const QModelIndex& parent)
    {
        beginInsertRows(parent, row, row + count - 1);
/*
        beginInsertRows(parent, row, row + count - 1);
        QVector<ldapcore::CLdapAttribute> attrs = m_entry->availableAttributes();
        for(int i=0;i<std::min(count, attrs.size());i++)
        {
            ldapcore::CLdapAttribute attr(attrs[i].name(),attrs[i].value(),attrs[i].type(), attrs[i].editState());
            m_pAttributes->append(attr);
        }
        */

        endInsertRows();
        m_IsChanged = true;

        QModelIndex idxFrom = index(row, 0, parent);
        QModelIndex idxTo   = index(row + count - 1, m_SectionsList.size()-1, parent);
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
        if(!m_pAttributes) return true;

        beginRemoveRows(parent, row, row + count-1);
         m_pAttributes->remove(row, count);
        endRemoveRows();
        m_IsChanged = true;
        return true;
    }

} //namespace ldapeditor
