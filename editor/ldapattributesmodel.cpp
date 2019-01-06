#include "ldapattributesmodel.h"

namespace ldapeditor
{
    CLdapAttributesModel::CLdapAttributesModel(QSet<QString>& uniqueDNs, QObject *parent)
        : QAbstractTableModel(parent)
        , m_SectionsList{ tr("Dn"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
        , m_uniqueDNs(uniqueDNs)
    {
    }

    QStringList CLdapAttributesModel::attributesList() const
    {
        return buildAttributesList(m_attributes);
    }

    void CLdapAttributesModel::setAttributesList(const QStringList& list)
    {
        beginResetModel();
        m_attributes.clear();
        removeRows(0, rowCount());

        for(const QString& attr : list)
        {
            QStringList values = attr.trimmed().split("=");
            if(values.size()>1)
            {
                LDapAttributeItem item;
                item.dn = attr;
                item.attr = values[0].trimmed();
                item.value = values[1].trimmed();
                item.type = tr("Text");
                item.size = QString::number(item.value.length()) ;
                m_attributes.append(item);
            }
        }
        endResetModel();
        emit dataChanged(index(0,0), index(m_attributes.size(),m_SectionsList.size()), QVector<int>() << Qt::DisplayRole);
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
        return m_attributes.size();
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
        if(role != Qt::DisplayRole) return QVariant();
        switch(index.column())
        {
        case 0: return m_attributes[index.row()].dn;
        case 1: return m_attributes[index.row()].attr;
        case 2: return m_attributes[index.row()].value;
        case 3: return m_attributes[index.row()].type;
        case 4: return m_attributes[index.row()].size;
        }
        return QVariant();
    }

    bool CLdapAttributesModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid())
            return false;

        if(index.column() == 2 && role == Qt::EditRole)
        {
            if (!value.toString().trimmed().isEmpty())
            {
                if (data(index, role) != value) {
                    // FIXME: Implement me!
                    tLdapAttributes tmpAttributes (m_attributes);
                    auto& a = tmpAttributes[index.row()];
                    a.value = value.toString().trimmed();
                    a.size = QString::number(a.value.length());
                    a.dn = QString("%1=%2").arg(a.attr).arg(a.value);

                    if(verifyAttributes(tmpAttributes, index.row()))
                    {
                        m_attributes = tmpAttributes;
                        m_IsChanged = true;
                        emit dataChanged(index, index, QVector<int>() << role);
                        return true;
                    }
                }
            }
        }

        return false;
    }

    Qt::ItemFlags CLdapAttributesModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags defaultFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable};
        Qt::ItemFlags editFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable};
        if (index.column() == 2)
        {
            QModelIndex idxAttr = index.model()->index(index.row(), 1, index.parent());
            if(idxAttr.isValid())
            {
                if(idxAttr.data(Qt::DisplayRole).toString().trimmed() != "dc")
                    return editFlags;
            }
        }
        return defaultFlags;
    }

    bool CLdapAttributesModel::insertRows(int row, int count, const QModelIndex &parent)
    {
        beginInsertRows(parent, row, row + count - 1);
        // FIXME: Implement me!
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

    bool CLdapAttributesModel::verifyAttributes(const tLdapAttributes& tmpAttributes, int row) const
    {
        bool bRet {true};
        const auto& a = tmpAttributes[row];
        QString newDN= buildDN(tmpAttributes).toLower();
        bRet &=  a.attr.toLower() != "dc";
        //bRet &=  !m_uniqueDNs.contains(newDN);
        return bRet;

    //    QString dn =  buildDN(tmpAttributes);
    //    return dn.contains(m_baseDN);
    }

    QStringList CLdapAttributesModel::buildAttributesList(const tLdapAttributes& tmpAttributes) const
    {
        QStringList attributes;
        for(auto a: tmpAttributes)
        {
            attributes << a.dn;
        }
        return attributes;
    }

    QString CLdapAttributesModel::buildDN(const tLdapAttributes& tmpAttributes) const
    {
        return buildAttributesList(tmpAttributes).join(", ").toLower();
    }

} //namespace ldapeditor
