/*!
\file
\brief Header for utility functions

File contains  declarations for utility functions
*/

#ifndef UTILITIES_H
#define UTILITIES_H
#include <type_traits>

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
 * \tparam Container type of container, must contain ldapcore::CLdapAttribute data
 * \param attributes container with ldapcore::CLdapAttribute data
 * \param attrName  name of attribute for search
 * \param caseSensitive  use CaseSensitive ot InCaceSensitive compare names
 */
template< class Container,
          class = typename std::enable_if< std::is_base_of<ldapcore::CLdapAttribute, typename Container::value_type>::value >::type >
bool containsAttribute(const Container& attributes, const QString& attrName, bool caseSensitive = false)
{
    return std::find_if(std::begin(attributes), std::end(attributes), [attrName, caseSensitive](const ldapcore::CLdapAttribute& a)
            {
                return a.name().compare(attrName, caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0;
            }) != std::end(attributes);
}
}

#endif // UTILITIES_H
