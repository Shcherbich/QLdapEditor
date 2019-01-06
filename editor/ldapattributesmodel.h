#ifndef CLDAPATTRIBUTESMODEL_H
#define CLDAPATTRIBUTESMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSet>

namespace ldapeditor
{
    class CLdapAttributesModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        explicit CLdapAttributesModel(QSet<QString>& uniqueDNs, QObject *parent = nullptr);

        QStringList attributesList() const;
        void setAttributesList(const QStringList& list);
        bool IsChanged() const {return m_IsChanged;}
        void setBaseDN(const QString& baseDN){m_baseDN = baseDN.toLower();}


        // Header:
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

        // Basic functionality:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        // Editable:
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole) override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        // Add data:
        bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
        bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    private:
        typedef struct tLDapAttributeItem{
            QString dn;
            QString attr;
            QString value;
            QString type;
            QString size;
        } LDapAttributeItem;
        typedef QVector<LDapAttributeItem> tLdapAttributes;

        bool verifyAttributes(const tLdapAttributes& tmpAttributes, int row) const;
        QStringList buildAttributesList(const tLdapAttributes& tmpAttributes) const;
        QString buildDN(const tLdapAttributes& tmpAttributes)const;

        tLdapAttributes m_attributes;
        QStringList m_SectionsList;
        bool m_IsChanged{false};
        QString m_baseDN;
        QSet<QString>& m_uniqueDNs;
    };
} //namespace ldapeditor

#endif // CLDAPATTRIBUTESMODEL_H
