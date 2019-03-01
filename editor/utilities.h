/*!
\file
\brief Header for utility functions

File contains  declarations for utility functions
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QListWidget>

namespace ldapeditor
{
/*!
 * \brief utility function returns user name from DN string
 * \param dn DN string
 * \return user name string
 */
QString userNameFromDN(const QString dn);


/*!
 * \brief utility function for move items bettween lists
 * \param sourceList pointer to QListWidget for moving item from
 * \param destList pointer to QListWidget for moveing item to
 */
void moveItemBettwenLists(QListWidget* sourceList, QListWidget* destList);

}

#endif // UTILITIES_H
