/*!
\file
\brief Header file for applications's main window

File contains  declarations for applications's main window
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QModelIndex>
#include <QSet>
#include "ldaptreeview.h"
#include "ldaptableview.h"

class QStandardItem;
class QSettings;
class QLabel;


namespace ldapcore {
class CLdapData;
}


/*!
 @defgroup ldapeditor
 @brief namespace for groupping editor related functionality

 Namespace ldapeditor wrapes all classes and declarations, related to editor functionality
*/
namespace ldapeditor
{
    class CLdapTreeModel;
    class CLdapTreeProxyModel;
    class CLdapAttributesModel;
    class CLdapSettings;

    /*!
     * @ingroup ldapeditor
     * @brief The MainWindow class
     *
     * Application's main window class
     */
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        /*!
         * @brief Constructor MainWindow
         * @param settings application settings
         * @param ldapData ldapcore::CLDapData reference object
         * @param parent pointer to parent QWidget object
         */
        MainWindow(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget* parent = nullptr);

        /*!
          * \brief Destructor MainWindow
         */
        ~MainWindow();

    signals:
        /*!
         * \brief Will be sent when need to delete entity from tree model
         * \param index index of entity to delete
         */
        void removeEntity(const QModelIndex index);

    protected slots:
        /*!
          @brief Handler for 'About Application' signel

          Method handles 'About Application' signal and show information about application
        */
        void onAboutApp();

        /*!
         * @brief onTreeItemChanged handler for signal about changing current item in Ldap Tree
         * @param current new tree item
         * @param previous previous tree item
         *
         * Method handles changing of current item in Ldap tree and initialize Atrributes list model with bew data
         */
        void onTreeItemChanged(const QModelIndex& current, const QModelIndex& previous);

        /*!
          @brief Handler for 'Ldap Search' signel

          Method handles 'Ldap Search' signal and brings up Ldap Search dialog
        */
        void onLdapSearch();

        /*!
          @brief Handler for 'Save Data' signel

          Method handles 'Save Data' signal and saves locally changed data to LDAP server
        */
        void onSaveData();

        /*!
          @brief Handler for 'Quit' signel

          Method handles 'Quit' signal and saves locally changed data to LDAP server before exit from application
        */
        void onQuit();

        /*!
          @brief Handler for 'Reconnect' signel

          Method handles 'Reconnect' signal and re-connects to LDAP server
        */
        void onReconnect();

        /*!
          @brief Handler for 'Reload Data' signel

          Method handles 'Reload Data' signal and re-reads LDAP tree from server
        */
        void onReload();

    protected:

        /*!
          @brief Creates Docked control with LDAP Tree

          Method creates docked control with LDAP tree
        */
        void CreateDockWindows();

        /*!
          @brief Creates application actions

          Method creates application actions and initilaize menu
        */
        void CreateActions();

        /*!
          @brief Creates status bar control

          Method creates status bar control
        */
        void CreateStatusBar();

        /*!
          @brief Updates availability of application actions

          Method updates availability of application's actions
        */
        void UpdateActionStatus();

        /*!
         * \brief Normalize DN string
         * \param dn source DN string
         * \return normalized DN string
         */
        QString normilizeDN(const QString& dn);

        /*!
          @brief member pointer to LDAP tree control
         */
        CLdapTreeView* m_LdapTree{nullptr};

        /*!
          @brief member pointer to LDAP attributes table control
         */
        CLdapTableView* m_AttributesList{nullptr};

        /*!
          @brief member pointer to LDAP tree model
         */
        CLdapTreeModel* m_TreeModel{nullptr};

        /*!
          @brief member pointer to LDAP tree proxy model
         */
        CLdapTreeProxyModel* m_TreeProxyModel{nullptr};

        /*!
          @brief member pointer to LDAP table model
         */
        CLdapAttributesModel* m_TableModel{nullptr};

        /*!
          @brief member LDAP root index for tree control
         */
        QModelIndex m_RootIndex;

        /*!
          @brief member list of unique DNs
         */
        QSet<QString> m_uniqueDNs;

        /*!
          @brief member boolean flag, controlsdoes we have connection to LDAP server
         */
        bool m_IsConnected{false};

        /*!
          @brief member application settings
         */
        CLdapSettings& m_Settings;

        /*!
          @brief member LDAP data
         */
        ldapcore::CLdapData& m_LdapData;

        /*!
          @brief member pointer QLabel about connected LDAP server(IP:port), shown in status bar
         */
        QLabel* m_IpStatus;
    };
} //namespace ldapeditor
#endif // MAINWINDOW_H
