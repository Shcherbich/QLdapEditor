/*!
\file
\brief Header file for LDAP tree view

File contains  declarations for LDAP tree view
*/

#ifndef LDAPTREEVIEW_H
#define LDAPTREEVIEW_H


#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <tuple>
#include "CLdapEntry.h"

namespace ldapcore
{
class CLdapData;
}


namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief The LDAP tree view class
 *
 * The LDAP tree view class
 */
class CLdapTreeView : public QTreeView
{
	Q_OBJECT
public:
    /*!
     * @brief Constructor CLdapTreeView
     * @param parent pointer to parent QWidget object
     * @param ldapData reference to ldapcore::CLdapData
     */
    CLdapTreeView(QWidget* parent, ldapcore::CLdapData& ldapData);

    /*!
     * @brief Methods finds item By DN string
     * @param dn string DN
     * @return std::tuple<QModelIndex, ldapcore::CLdapEntry*> of found item if success, or Invalid Index with nullptr, if item not found
     */
	std::tuple<QModelIndex, ldapcore::CLdapEntry*> findByDn(QString dn);

signals:
    /*!
     * @brief Signal about changing current item in tree
     * @param current QModelIndex of new current item
     * @param previous QModelIndex of previous item
     */
    void treeItemChanged(const QModelIndex& current, const QModelIndex& previous);

    /*!
     * @brief Signal about adding new attribute to  entity
     * @param attr added attribute
     */
    void onAddAttribute(const ldapcore::CLdapAttribute& attr);

    /*!
     * @brief Signal about removing new attribute from  entity
     * @param attr removed attribute
     */
    void onRemoveAttribute(const ldapcore::CLdapAttribute& attr);

protected :
    /*!
     * @brief protected method called when current item is changed
     * @param current QModelIndex of new current item
     * @param previous QModelIndex of previous item
     */
	virtual void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

    /*!
     * \brief Draws the branches in the tree view on the same row as the model item index, using the painter given.
     * \param painter pointer to QPainter class
     * \param rect rectangle inside tree row where branch will be painted
     * \param index row index
     *
     * Draws the branches in the tree view on the same row as the model item index, using the painter given. The branches are drawn in the rectangle specified by rect.
     */
    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

public slots:
    /*!
     * \brief protected slot, Expands the model item specified by the index.
     * \param index index of item to expand
     */
    void  expand(const QModelIndex &index);

protected slots:
    /*!
     * @brief Method is called when custom contect menu is requested (e.g. by mouse right click)
     * @param pos point where click has happened
     */
	void customContextMenuRequested(QPoint pos);

    /*!
     * @brief Method is called by 'New Entry' command
     */
	void onNewEntry();

    /*!
     * @brief Method is called by 'Edit Entry' command
     */
    void onEditEntry();

    /*!
     * @brief Method is called by 'Delete Entry' command
     */
    void onDeleteEntry();

    /*!
     * @brief Method is called by 'Change password' command
     */
    void onChangePassword();
private:    
    QMenu       m_contextMenu;          ///< Context menu pointer member
    QAction*    m_newEntry{nullptr};    ///< 'New Entry' action member
    QAction*    m_editEntry{nullptr};   ///< 'Edit Entry' action member
    QAction*    m_deleteEntry{nullptr}; ///< 'Delete Entry' action member
    QAction*    m_changePassword{nullptr}; ///< 'Change password' action member

    ldapcore::CLdapData& m_LdapData;    ///< ldapcore::CLdapData reference member
};

} //namespace ldapeditor


#endif // LDAPTREEVIEW_H
