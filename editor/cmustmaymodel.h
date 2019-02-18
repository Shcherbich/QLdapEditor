/*!
@file
@brief Header file for may/must attributes model class

File contains  declarations for may/must attributes model class
*/

#ifndef CMUSTMAYMODEL_H
#define CMUSTMAYMODEL_H


#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include "CLdapAttribute.h"
#include "CLdapEntry.h"

namespace ldapeditor{

/*!
 * @ingroup ldapeditor
 * @brief May/Must attributes model class
 *
 * Class provides may/must attributes model
 *
 */
class MustMayModel : public QAbstractListModel
{
   Q_OBJECT
public:
    /*!
    * @brief Constructor MustMayModel class
    * @param parent pointer to QObject parent
    * @param entry pointer to ldapcore::CLdapEntry
    * @param existAttributes array of exist attributes
    */
   MustMayModel(QObject* parent, ldapcore::CLdapEntry* entry, QVector<ldapcore::CLdapAttribute>* existAttributes);

   /*!
    * @brief Returns the number of rows under the given parent.
    * @param parent parent item index
    * @return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
    */
   int rowCount(const QModelIndex &parent = QModelIndex()) const;

   /*!
    * @brief Returns the data stored under the given role for the item referred to by the index.
    * @param index item index
    * @param role  role id ,reference to requested data
    * @return Returns the QVariant of data stored under the given role for the item referred to by the index.
    */
   QVariant data(const QModelIndex &index, int role) const;

private:
   QVector<ldapcore::CLdapAttribute> must; ///< Must attributes array
   QVector<ldapcore::CLdapAttribute> may;  ///< May attributes array

};

} //namespace ldapeditor

#endif // CMUSTMAYMODEL_H
