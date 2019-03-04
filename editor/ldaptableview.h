/*!
@file
@brief Header file for LDAP attributes view class

File contains  declarations for LDAP attributes view class
*/

#ifndef LDAPTABLEVIEW_H
#define LDAPTABLEVIEW_H

#include <QTableView>
#include <QStyledItemDelegate>
//#include <QSortFilterProxyModel>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "ldapdataeditdelegate.h"
#include "CLdapEntry.h"
#include "ldapsettings.h"

namespace ldapeditor
{
    class CLdapAttributesProxyModel;
    class CLdapAttributesModel;

    /*!
     * @ingroup ldapeditor
     * @brief LDAP attributes view class
     *
     * Class provides LDAP attributes view functionality
     *
     */
    class CAttributesList : public QTableView
    {
        Q_OBJECT
    public:

        /*!
         * \brief Constructor CLdapTableView
         * \param parent pointer to parent QWidget
         * \param s  reference to CLdapSettings
         */
        explicit CAttributesList(QWidget *parent, ldapcore::CLdapData&, CLdapSettings& s);

        /*!
         * \brief Method sets pointer to CLdapEntry
         * \param entry pointer to CLdapSettings
         */
        void setLdapEntry(ldapcore::CLdapEntry* entry);

        /*!
         * \brief Method restores view settings
         */
        void RestoreView();

        /*!
         * \brief Method saves attributes data
         * \return true if data saved, false - if not
         */
        bool SaveData();

    signals:

    public slots:
        /*!
         * \brief Public slot, called when user requested show of Context menu on item
         * \param pos position in view for context menu
         */
      //  void customContextMenuRequested(QPoint pos);

        /*!
         * \brief Public slot, called by 'Manage User in group' action
         *
         * Also can be called by manageUserIngroup signal from CLdapTree view
         */
        void onManageUsersInGroup();

    protected slots:

        /*!
         * \brief Public slot, called when user requested show of Context menu on item
         * \param pos position in view for context menu
         */
        void customContextMenuRequested(QPoint pos);

        /*!
         * \brief Protected slot, called by 'New Attribute' action
         */
        void onNewAttribute();

        /*!
         * \brief Protected slot, called by 'Delete Attribute' action
         */
        void onDeleteAttribute();


    protected:

        /*!
         * \brief Protected method starts editing the item at index, creating an editor if necessary
         * \param index index of item for edit
         * \param trigger actions which will initiate item editing
         * \param event associated event
         * \return if the view's State is now EditingState; otherwise returns false.
         */
        virtual bool edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)override;

        /*!
         * \brief Protected method receives mouse press events for the widget.
         * \param event parameters that describe a mouse event
         */
        void mousePressEvent(QMouseEvent *event)override;

        /*!
         * \brief Protected method calld when header section is changed
         * \param logicalIndex logical index of column
         * \param oldSize old size of column
         * \param newSize new size of column
         */
        void OnHeaderChanged(int logicalIndex, int oldSize, int newSize);

        CLdapDataEditDelegate m_ldapDataDelegate;   ///< Ldap data delegate  editor member
        QStyledItemDelegate   m_defaultDelegate;    ///< Default data delegate edtitor member
        ldapcore::CLdapData&  m_LdapData;           ///< Reference to CLdapData member
        CLdapSettings&        m_LdapSettings;       ///< Referencve t oapplication's settings member
        QMenu m_contextMenu;                        ///< Context menu instance member
        QAction* m_newAttr{nullptr};                ///< 'New Attribute' action pointer member
        QAction* m_delAttr{nullptr};                ///< 'Delete Attribute' action pointer member

        ldapcore::CLdapEntry* m_entry{nullptr};     ///< CLdapEntry data pointer
    };

    /*!
     * @ingroup ldapeditor
     * @brief LDAP view class
     *
     * Class provides LDAP attributes view functionality
     *
     */
    class CLdapTableView : public QWidget
    {
        Q_OBJECT
    public:

        /*!
         * \brief Constructor CLdapTableView
         * \param parent pointer to parent QWidget
         * \param s  reference to CLdapSettings
         */
        explicit CLdapTableView(QWidget *parent, ldapcore::CLdapData&, CLdapSettings& s);

        void setModel(QAbstractItemModel *model);

        /*!
         * \brief Method sets pointer to CLdapEntry
         * \param entry pointer to CLdapSettings
         */
        void setLdapEntry(ldapcore::CLdapEntry* entry);

        /*!
         * \brief Method restores view settings
         */
        void RestoreView();

        /*!
         * \brief Method saves attributes data
         * \return true if data saved, false - if not
         */
        bool SaveData();

    public slots:

        /*!
         * \brief Public slot, called by 'Manage User in group' action
         *
         * Also can be called by manageUserIngroup signal from CLdapTree view
         */
        void onManageUsersInGroup();


    protected:
        QVBoxLayout*     m_layout {nullptr};    ///< layout controls member
        QLineEdit*       m_filterEdit{nullptr}; ///< filter edit  member
        CAttributesList* m_attrList{nullptr};   ///< attributes list member
        CLdapAttributesProxyModel* m_proxyModel{nullptr};
    };
} //namespace ldapeditor
#endif // LDAPTABLEVIEW_H
