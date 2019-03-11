/*!
\file
\brief Header for utility functions

File contains  declarations for utility functions
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QListWidget>
#include "CLdapAttribute.h"

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

/*!
 * \brief utility function for checkign existing attribute with name in container
 * \param Container type of container, must contain ldapcore::CLdapAttribute
 * \param name  name otf attribute foir search
 * \param caseSensitive  use CaseSansitive ot InCaceSansitive compare names
 */
template<class Container>
bool containsAttribute(const Container& v, const QString& name, bool caseSensitive = false)
{
    return std::find_if(std::begin(v), std::end(v), [&](const ldapcore::CLdapAttribute & a)
            {
                return a.name().compare(name, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0;
            }) != std::end(v);
}
}

#endif // UTILITIES_H
