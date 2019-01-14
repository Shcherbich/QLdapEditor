#include "ldapattributesmodel.h"
#include "ldapeditordefines.h"
#include <QDate>
#include <QTime>
namespace ldapeditor
{
    CLdapAttributesModel::CLdapAttributesModel(QObject *parent)
        : QAbstractTableModel(parent)
        , m_SectionsList{ tr("Dn"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
    {
    }


    void CLdapAttributesModel::setAttributesList(QVector<ldapcore::CLdapAttribute>* pAttrs)
    {
        beginResetModel();
        if(m_pAttributes)
            m_pAttributes->clear();
        removeRows(0, rowCount());

        m_pAttributes = pAttrs;
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
        if (parent.isValid())
            return 0;

        // FIXME: Implement me!
        return m_pAttributes ? m_pAttributes->size() : 0;
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

        ldapcore::AttributeState editState = (*m_pAttributes)[index.row()].editState();
        switch(editState)
        {
        case ldapcore::AttributeState::AttributeReadOnly:
            return readonlyFlags; break;
        case ldapcore::AttributeState::AttributeReadWrite:
            return index.column() > 0 && index.column() != 4 ? editFlags:readonlyFlags; break;
        case ldapcore::AttributeState::AttributeValueReadWrite:
            return index.column() == 2 ? editFlags:readonlyFlags; break;
        default: break;
        }
        return readonlyFlags;
    }

    bool CLdapAttributesModel::insertRows(int row, int count, const QModelIndex &parent)
    {
//        beginResetModel();
//        ldapcore::CLdapAttribute attr("newAttr","newValue",ldapcore::AttrType::UnknownText, ldapcore::AttributeState::AttributeReadWrite);
//        m_pAttributes->push_back(attr);
//        endResetModel();
//        emit dataChanged(index(0,0), index(m_pAttributes->size(),m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);

        beginInsertRows(parent, row, row + count - 1);
        // FIXME: Implement me!
        ldapcore::CLdapAttribute attr("newAttr","newValue",ldapcore::AttrType::UnknownText, ldapcore::AttributeState::AttributeReadWrite);
        m_pAttributes->push_back(attr);
        m_IsChanged = true;
        endInsertRows();

        return false;
    }

    bool CLdapAttributesModel::insertColumns(int column, int count, const QModelIndex &parent)
    {
        beginInsertColumns(parent, column, column + count - 1);
        // FIXME: Implement me!
        endInsertColumns();
        return false;
    }

    bool CLdapAttributesModel::removeRows(int row, int count, const QModelIndex &parent)
    {
        if(!m_pAttributes) return true;

        beginRemoveRows(parent, row, row + count-1);
        if(row>=0 && row < m_pAttributes->size())
        {
            m_pAttributes->remove(row, count);
        }
        endRemoveRows();
        return true;
    }

} //namespace ldapeditor
