#ifndef CLDAPATTRIBUTESMODEL_H
#define CLDAPATTRIBUTESMODEL_H


#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QString>
#include "CLdapAttribute.h"
#include "CLdapEntry.h"
#include "attributemodelhelper.h"

namespace ldapeditor
{
    class CLdapAttributesModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        explicit CLdapAttributesModel(ldapcore::CLdapData& ldapData, QObject *parent);

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

        void sortData();
        void GetChangedRows(QVector<ldapcore::CLdapAttribute>& newRows, QVector<ldapcore::CLdapAttribute>& deleteRows, QVector<ldapcore::CLdapAttribute>& updateRows);
        bool Save();
        bool isNew()    const;
        bool isEdit()   const;
        QString dn()    const;

        QModelIndex addAttribute(const ldapcore::CLdapAttribute& attribute);

    public slots:
        void onRemovingAttribute(QString name);
        void onAddAttribute(QString name);

    protected:
        bool  SaveAttributes();
        bool  SaveNewEntry();
        bool  SaveUpdatedEntry();

    private:
        ldapcore::CLdapData& m_LdapData;
        QStringList m_SectionsList;
        bool m_IsChanged{false};
        QString m_baseDN;
        ldapcore::CLdapEntry* m_entry{nullptr};
        QVector<ldapcore::CLdapAttribute>* m_pAttributes{nullptr};
        CAttributeModelHelper m_attrHelper;
    };
} //namespace ldapeditor

#endif // CLDAPATTRIBUTESMODEL_H
