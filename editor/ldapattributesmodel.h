#ifndef CLDAPATTRIBUTESMODEL_H
#define CLDAPATTRIBUTESMODEL_H


#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include "CLdapAttribute.h"
#include "CLdapEntry.h"
#include "attributemodelhelper.h"

namespace ldapeditor
{
    class CLdapAttributesModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        explicit CLdapAttributesModel(QObject *parent);

        void setLdapEntry(ldapcore::CLdapEntry* entry);
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

        // Delete Data
        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) ;

        // save data to server
        void SaveToServer();

    private:
        QStringList m_SectionsList;
        bool m_IsChanged{false};
        QString m_baseDN;
        ldapcore::CLdapEntry* m_entry{nullptr};
        QVector<ldapcore::CLdapAttribute>* m_pAttributes{nullptr};
        CAttributeModelHelper m_attrHelper;
    };
} //namespace ldapeditor

#endif // CLDAPATTRIBUTESMODEL_H
