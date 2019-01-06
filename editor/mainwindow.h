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

namespace ldapeditor
{
    class CLdapTreeModel;
    class CLdapAttributesModel;
    class CLdapSettings;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget* parent = nullptr);
        ~MainWindow();

    protected slots:
        void onAboutApp();
        void onTreeItemChanged(const QModelIndex& current, const QModelIndex& previous);
        void onLdapSearch();

    protected:
        void CreateDockWindows();
        void CreateActions();
        void CreateStatusBar();
        void UpdateActionStatus();

        QString normilizeDN(const QString& dn);
        QStandardItem* prepareRootItem(const QString& dn, const QString& host);
        QStandardItem* prepareDataForDn(const QString& dn, QStandardItem* parentItem);

        CLdapTreeView* m_LdapTree{nullptr};
        CLdapTableView* m_AttributesList{nullptr};

        CLdapTreeModel* m_TreeModel{nullptr};
        CLdapAttributesModel* m_TableModel{nullptr};
        QModelIndex m_RootIndex;
        QSet<QString> m_uniqueDNs;

        bool m_IsConnected{false};
        CLdapSettings& m_Settings;
        ldapcore::CLdapData& m_LdapData;
        QLabel* m_IpStatus;
    };
} //namespace ldapeditor

#endif // MAINWINDOW_H
