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

    QVector<ldapcore::CLdapAttribute> CLdapAttributesModel::attributesList() const
    {
        return m_attributes;
    }

    void CLdapAttributesModel::setAttributesList(QVector<ldapcore::CLdapAttribute>& attrs)
    {
        beginResetModel();
        m_attributes.clear();
        removeRows(0, rowCount());

        m_attributes = attrs;
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
        ldapcore::CLdapAttribute const &  attr = m_attributes[index.row()];
        if(role == Qt::DisplayRole)
        {
            QString value = formatValueByType(attr);
            QString length = QString::number(attr.value().length());
            switch(index.column())
            {
            case 0: return QString("%1=%2").arg(attr.name()).arg(attr.value());
            case 1: return attr.name();
            case 2: return value;
            case 3: return attr.typeAsString();
            case 4: return length;
            }
        }
        else if(role ==Qt::EditRole)
        {
            if(index.column() == 2)
            {
                switch(attr.type())
                {
                case ldapcore::AttrType::Date:
                    return QDate::fromString(attr.value()); break;
                case ldapcore::AttrType::Time:
                    return QTime::fromString(attr.value()); break;
                case ldapcore::AttrType::Int:
                     return attr.value().toInt(); break;
                default:
                    return attr.value();
                }
            }
        }
        else if(role == AttrTypeRole)
        {
            return static_cast<int>(attr.type());
        }
        return QVariant();
    }

    bool CLdapAttributesModel::setData(const QModelIndex &index, const QVariant &value, int role)
    {
        if (!index.isValid())
            return false;

//        if(index.column() == 2 && role == Qt::EditRole)
//        {
//            if (!value.toString().trimmed().isEmpty())
//            {
//                if (data(index, role) != value) {
//                    // FIXME: Implement me!
//                    tLdapAttributes tmpAttributes (m_attributes);
//                    auto& a = tmpAttributes[index.row()];
//                    a.value = value.toString().trimmed();
//                    a.size = QString::number(a.value.length());
//                    a.dn = QString("%1=%2").arg(a.attr).arg(a.value);

//                    if(verifyAttributes(tmpAttributes, index.row()))
//                    {
//                        m_attributes = tmpAttributes;
//                        m_IsChanged = true;
//                        emit dataChanged(index, index, QVector<int>() << role);
//                        return true;
//                    }
//                }
//            }
//        }

        return false;
    }

    QString CLdapAttributesModel::formatValueByType(const ldapcore::CLdapAttribute& attr) const
    {
        QString retValue;
        switch(attr.type())
        {
        case ldapcore::AttrType::Int:
            retValue = attr.value();
            break;
        case ldapcore::AttrType::Binary:
            {
                QByteArray a = attr.value().toLocal8Bit().toHex();
                for(int i=0;i<a.size();i+=2)
                {
                    if(!retValue.isEmpty())
                        retValue += " ";
                    retValue+= QString("%1%2").arg(QChar(a[i]).toUpper()).arg(QChar(a[i+1]).toUpper());
                }
            }
            break;
        case ldapcore::AttrType::String:
        case ldapcore::AttrType::Date:
        case ldapcore::AttrType::Time:
            retValue = attr.value();
            break;
        }
        return retValue;
    }

    Qt::ItemFlags CLdapAttributesModel::flags(const QModelIndex &index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        Qt::ItemFlags defaultFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable};
        Qt::ItemFlags editFlags{Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable};
        ldapcore::CLdapAttribute const &  attr = m_attributes[index.row()];

        if (index.column() == 2) //do not allow edit binary data
        {
            return attr.type() == ldapcore::AttrType::Binary ? defaultFlags : editFlags;
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



//    bool CLdapAttributesModel::verifyAttributes(const tLdapAttributes& tmpAttributes, int row) const
//    {
//        bool bRet {true};
//       // const auto& a = tmpAttributes[row];
//        //QString newDN= buildDN(tmpAttributes).toLower();
//        //bRet &=  a.attr.toLower() != "dc";
//        //bRet &=  !m_uniqueDNs.contains(newDN);
//        return bRet;

//    }

//    QStringList CLdapAttributesModel::buildAttributesList(const tLdapAttributes& tmpAttributes) const
//    {
//        QStringList attributes;
//        for(auto a: tmpAttributes)
//        {
//            attributes << a.dn;
//        }
//        return attributes;
//    }

//    QString CLdapAttributesModel::buildDN(const tLdapAttributes& tmpAttributes) const
//    {
//        return buildAttributesList(tmpAttributes).join(", ").toLower();
//    }

} //namespace ldapeditor
