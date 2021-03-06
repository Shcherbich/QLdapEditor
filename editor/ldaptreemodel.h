/*!
\file
\brief Header file for LDAP tree models

File contains  declarations for LDAP tree models
*/
#ifndef CLDAPTREEMODEL_H
#define CLDAPTREEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QVector>
#include <QString>

namespace ldapcore {
class CLdapData;
class CLdapEntry;
class CLdapAttribute;
}

namespace ldapeditor
{

    /*!
     * @ingroup ldapeditor
     * @brief The LDAP tree proxy model class
     *
     * The LDAP tree proxy model class, used for providing sorted tree items
     */
    class CLdapTreeProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        /*!
         * @brief Constructor of CLdapTreeProxyModel
         * @param parent pointer to parent QObject
         */
        explicit CLdapTreeProxyModel(QObject *parent = nullptr):QSortFilterProxyModel(parent)
        {
        }

        /*!
         * \brief Method for gettting list of persistent indexes
         * \return QModelIndexList with persistent indexes
         */
        QModelIndexList getPersistentIndexList() {
            return persistentIndexList();
        }
    protected:
        /*!
         * @brief Protected method, used for sort model itens
         * @param source_left - 1st model item
         * @param source_right - 2nd model item
         * @return true if source_left should be placed before source_right, and false if not
         */
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    };

    /*!
     * @ingroup ldapeditor
     * @brief The LDAP tree model class
     *
     * The LDAP tree model class, provides data for tree view
     */
    class CLdapTreeModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        /*!
         * @brief Constructor CLdapTreeModel
         * @param ldapData reference to ldapcore::CLdapData
         * @param parent pointer to parent QObject
         */
        explicit CLdapTreeModel(ldapcore::CLdapData& ldapData, QObject *parent = nullptr);

        /*!
         * @brief Method sets top items for model
         * @param topItems vector of pointers to top items of dapcore::CLdapEntry*
         * @return true if success, false if error
         */
        bool setTopItems(QVector<ldapcore::CLdapEntry*> topItems);

        // Basic functionality:
        /*!
         * @brief Returns the index of the item in the model specified by the given row, column and parent index.
         * @param row      row index for item
         * @param column   column index for item
         * @param parent   parent index
         * @return  Returns the index of the item in the model specified by the given row, column and parent index.
         */
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        /*!
         * @brief Returns the parent of the model item with the given index. If the item has no parent, an invalid QModelIndex is returned.
         * @param index index of item
         * @return  Returns the parent of the model item with the given index. If the item has no parent, an invalid QModelIndex is returned.
         */
        QModelIndex parent(const QModelIndex &index) const override;

        /*!
         * @brief Returns the number of rows under the given parent.
         * @param parent parent item index
         * @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
         */
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        /*!
         * @brief Returns the number of columns for the children of the given parent.
         * @param parent parent item index
         * @return Returns the number of columns for the children of the given parent.
         */
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        /*!
         * @brief Returns the data stored under the given role for the item referred to by the index.
         * @param index item idex
         * @param role  role id ,reference to requested data
         * @return Returns the QVariant of data stored under the given role for the item referred to by the index.
         */
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        /*!
         * @brief Sets the role data for the item at index to value.
         * @param index item index
         * @param value value to be set
         * @param role role id for data to be set
         * @return Returns true if successful; otherwise returns false.
         */
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole) override;

        /*!
         * @brief Returns the item flags for the given index.
         * @param index item index
         * @return Returns the item flags for the given index.
         */
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        /*!
          @brief Method removes attribute by name
        */
        Q_INVOKABLE void removeAttribute(QString name);

        /*!
          @brief Method adds attribute by name
        */
        Q_INVOKABLE void addAttribute(QString name);

        /*!
         * @brief Method adds new Entity
         * @param parent parent entity
         * @param rdn RDN string
         * @param dn  DN string
         * @param classes vector of classes for new entity
         * @param attributes vector of attributes for new entity
         * @return Index of new added entity in tree view
         */
        QModelIndex addNewEntry(QModelIndex parent, QString rdn, QString dn, QStringList& classes, QVector<ldapcore::CLdapAttribute>& attributes);

        /*!
         * \brief Method removes entity from LDAP Tree
         * \param row start row for deleting
         * \param count count of rows for deleting
         * \param parent parent index
         * \return true if success, false if else
         */
        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

        /*!
         * \brief Returns true if there is more data available for parent; otherwise returns false.
         * \param parent index of item
         * \return Returns true if there is more data available for parent; otherwise returns false.
         */
        bool canFetchMore(const QModelIndex &parent) const override;

        /*!
         * \brief Fetches any available data for the items with the parent specified by the parent index.
         * \param parent index of item
         */
        void fetchMore(const QModelIndex& parent) override;

        /*!
         * \brief Returns true if parent has any children; otherwise returns false.
         * \param parent index of  item
         * \return  Returns true if parent has any children; otherwise returns false.
         */
        bool hasChildren(const QModelIndex &parent) const;

        /*!
         * \brief Inserts count rows into the model before the given row. Items in the new row will be children of the item represented by the parent model index.
         * \param row  row where insert new records
         * \param count - count of new rows
         * \param parent parent index
         * \return true, if rows inserted, false - if not
         */
        bool insertRows(int row, int count, const QModelIndex& parent);
    protected:


    private:
        ldapcore::CLdapEntry*           m_invisibleRoot{nullptr}; ///< hidden root item for LDAP tree
        QVector<ldapcore::CLdapEntry*>  m_topItems;               ///< Top items for LDAP tree
        ldapcore::CLdapData& m_LdapData;                          ///< reference to ldapcore::CLdapData

    signals:
        /*!
         * @brief Signal sent when attribute is removed
         * @param name name of removed attribute
         */
        void onRemovingAttribute(QString name);

        /*!
         * @brief Signal sent when attribute is added
         * @param name name of added attribute
         */
        void onAddAttribute(QString name);

    public slots:
        /*!
         * \brief Public slot, called when need to remove entity from tree mdel by index
         * \param index index of entity to delete
         */
        void onRemoveEntity(const QModelIndex index);
    };
} //namespace ldapeditor


#endif // CLDAPTREEMODEL_H
