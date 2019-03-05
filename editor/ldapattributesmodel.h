/*!
@file
@brief Header file for LDAP Entity's attributes  model class

File contains  declarations for LDAP Entity's attributes model class
*/
#ifndef CLDAPATTRIBUTESMODEL_H
#define CLDAPATTRIBUTESMODEL_H


#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QString>
#include "CLdapAttribute.h"
#include "CLdapEntry.h"
#include "attributemodelhelper.h"

namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief The LDAP attributes proxy model class
 *
 * The LDAP attributes list proxy model class, used for providing filterring items
 */
class CLdapAttributesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /*!
     * @brief Constructor of CLdapAttributesProxyModel
     * @param parent pointer to parent QObject
     */
    explicit CLdapAttributesProxyModel(QObject *parent = nullptr):QSortFilterProxyModel(parent)
    {
    }

protected:
    /*!
     * @brief Protected method, used for filtering model itens
     * @param sourceRow - row model item
     * @param sourceParent - row model parent item
     * @return true if sourceRow should be shown in view, and false if not
     */
    virtual bool filterAcceptsRow(int sourceRow,  const QModelIndex &sourceParent) const;

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
 * @brief LDAP Entity's attributes model class
 *
 * Class provides LDAP Entity's attributes model
 *
 */
    class CLdapAttributesModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor CLdapAttributesModel
         * \param ldapData reference to ldapcore::CLdapData
         * \param parent pointer to parent QObject
         */
        explicit CLdapAttributesModel(ldapcore::CLdapData& ldapData, QObject *parent);

        /*!
         * \brief Method sets pointer to ldapcore::CLdapEntry and , in fact, intializes model
         * \param entry pointer t oldapcore::CLdapEntry
         */
        void setLdapEntry(ldapcore::CLdapEntry* entry);

        /*!
         * \brief Method returns flag is model changed
         * \return true, if model has been chnaged, false - if not
         */
        bool IsChanged() const {return m_IsChanged;}

        /*!
         * \brief Method sets base DN string
         * \param baseDN - Base DN string
         */
        void setBaseDN(const QString& baseDN){m_baseDN = baseDN.toLower();}


        // Header:
        /*!
         * \brief Returns the data for the given role and section in the header with the specified orientation.
         * \param section section index
         * \param orientation - vertical or horizontal header
         * \param role Role Id for requested data
         * \return data, according requested Role
         */
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        /*!
         * \brief Sets the data for the given role and section in the header with the specified orientation to the value supplied.
         * \param section section index
         * \param orientation vertical or horizontal header
         * \param value new value for data
         * \param role Role Id for set data
         * \return true if data are set, false - if not
         */
        bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

        // Basic functionality:
        /*!
         * @brief Returns the number of rows under the given parent.
         * @param parent parent item index
         * @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
         */
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;

        /*!
         * \brief Returns the number of columns for the children of the given parent.
         * \param parent parent item index
         * \return Returns the number of columns for the children of the given parent.
         */
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        /*!
         * @brief Returns the data stored under the given role for the item referred to by the index.
         * @param index item index
         * @param role  role id ,reference to requested data
         * @return Returns the QVariant of data stored under the given role for the item referred to by the index.
         */
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

        // Editable:
        /*!
         * \brief Sets the role data for the item at index to value.
         * \param index index of item
         * \param value new value t oset according Role Id
         * \param role Role Id ,reference to data
         * \return true if data are set, false - if not
         */
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole) override;

        /*!
         * \brief Returns the item flags for the given index.
         * \param index index of item
         * \return Returns the item flags for the given index.
         */
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        // Add data:
        /*!
         * \brief Inserts count rows into the model before the given row. Items in the new row will be children of the item represented by the parent model index.
         * \param row  row where insert new records
         * \param count - count of new rows
         * \param parent parent index
         * \return true, if rows inserted, false - if not
         */
        bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

        /*!
         * \brief Inserts count new columns into the model before the given column
         * \param column column where insert new columns
         * \param count count of new columns
         * \param parent parent index
         * \return return true, if columns inserted, false - if not
         */
        bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

        // Delete Data
        /*!
         * \brief Removes count rows starting with the given row under parent parent from the model.
         * \param row start index of rows for removing
         * \param count count of rows to be removed
         * \param parent parent index
         * \return true, if rows where removed, false - if not
         */
        bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) ;

        /*!
         * \brief Sorts data in model
         */
        void sortData();

        /*!
         * \brief Returns information about chnaged rows
         * \param newRows - number of new inserted rows
         * \param deleteRows - number of removed rows
         * \param updateRows - number of modified rows
         */
        void changedRows(QVector<ldapcore::CLdapAttribute>& newRows, QVector<ldapcore::CLdapAttribute>& deleteRows, QMap<QString, QVector<ldapcore::CLdapAttribute>>& updateMap);

        /*!
         * \brief Saves atttributes to server
         * \return true  if data saved, false - if not
         */
        bool Save();

        /*!
         * \brief Returns flag is of entity new
         * \return true if entity is new, false in not
         */
        bool isNew()    const;

        /*!
         * \brief Returns flag is entity is edited
         * \return true - entity is edited, false -if not
         */
        bool isEdit()   const;

        /*!
         * \brief Returns DN string
         * \return DN string
         */
        QString dn()    const;

        /*!
         * \brief Adds attribute to entity
         * \param attribute attribute to be added t oentity
         * \return QodelIndex of new added attribute
         */
        QModelIndex addAttribute(const ldapcore::CLdapAttribute& attribute);

    public slots:
        /*!
         * \brief Public slot, called when attibute is removed from entity
         * \param attr removed attribute
         */
        void onRemoveAttribute(const ldapcore::CLdapAttribute& attr);

        /*!
         * \brief Public slot, called when attribute is added
         * \param attr added attribute
         */
        void onAddAttribute(const ldapcore::CLdapAttribute& attr);

    protected:
        /*!
         * \brief Protected method , saves attributes
         * \return true if saved, false - if not
         */
        bool  SaveAttributes();

        /*!
         * \brief Protected method, saves new entity attributes
         * \return true if saved, false - if not
         */
        bool  SaveNewEntry();

        /*!
         * \brief Protected method, saves modified entity attributes
         * \return true if saved, false - if not
         */
        bool  SaveUpdatedEntry();

    private:
        ldapcore::CLdapData& m_LdapData;                        ///< reference to CLdapData member
        QStringList m_SectionsList;                             ///< sections (headers) titles member
        bool m_IsChanged{false};                                ///< flag is model changed memeber
        QString m_baseDN;                                       ///< Base DN string memeber
        ldapcore::CLdapEntry* m_entry{nullptr};                 ///< pointer to CLdapEntity member
        QVector<ldapcore::CLdapAttribute>* m_pAttributes{nullptr};///< pointer to entity's attributes array
        CAttributeModelHelper m_attrHelper;                     ///< attributes helper memeber
    };
} //namespace ldapeditor

#endif // CLDAPATTRIBUTESMODEL_H
