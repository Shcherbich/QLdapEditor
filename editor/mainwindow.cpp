/*!
\file
\brief Implementation file for applications's main window

File contains  implementations for applications's main window
*/

#include <algorithm>
#include "mainwindow.h"
#include "ldapeditordefines.h"
#include "ldapsearchdialog.h"
#include "ldaptreemodel.h"
#include "ldapattributesmodel.h"
#include "ldapsettings.h"
#include "common.h"

#include "CLdapData.h"

#include <QTreeView>
#include <QTableView>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDate>
#include <QLabel>
#include <QTreeView>

namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief The Auxiliary class locks updates of QWidgets
 *
 * Auxiliary class locks updates of QWidgets
 */
struct QtUiLocker
{
    QWidget* m_pWidget; ///< pointer to QWidget to be locked

    /*!
     * \brief Constructor QtUiLocker
     * \param pWidget pointer to QWidget which should be locked
     * Locks updates of QWidget
     */
    QtUiLocker(QWidget* pWidget)
        : m_pWidget(pWidget)
    {
        m_pWidget->setUpdatesEnabled(false);
    }

    /*!
      \brief Destructor of QtUiLocker
      Unlocks QWidget locked in constructor
    */
    ~QtUiLocker()
    {
        m_pWidget->setUpdatesEnabled(true);
    }

};

MainWindow::MainWindow(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget* parent)
	: QMainWindow(parent)
	, m_Settings(settings)
	, m_LdapData(ldapData)
{
	setWindowTitle(ApplicationName);
    setMinimumSize(800, 600);
    resize(1024, 800);

	CreateDockWindows();
	CreateActions();
	CreateStatusBar();

    QString baseDN = normilizeDN(m_Settings.baseDN());

    m_TreeModel = new CLdapTreeModel(m_LdapData, this);
    m_TreeProxyModel = new CLdapTreeProxyModel(this);

    m_TableModel = new CLdapAttributesModel(m_LdapData, this);
    m_AttributesList = new CLdapTableView(this, m_LdapData, m_Settings);

    setCentralWidget(m_AttributesList);


    m_TreeModel->setTopItems(m_LdapData.topList());

    m_TableModel->setBaseDN(baseDN);
    m_AttributesList->setModel(m_TableModel);

    m_AttributesList->RestoreView();

    m_TreeProxyModel->setDynamicSortFilter(true);
    m_TreeProxyModel->setSourceModel(m_TreeModel);
    m_TreeProxyModel->setSortRole(Qt::DisplayRole);
    m_TreeProxyModel->sort(0);
    m_LdapTree->setModel(m_TreeProxyModel);
    m_LdapTree->header()->resizeSection(0, m_LdapTree->header()->width());

    m_RootIndex = m_TreeProxyModel->index(0, 0);
    m_LdapTree->expand(m_RootIndex);
    m_LdapTree->setCurrentIndex(m_RootIndex);


    connect(m_LdapTree, &CLdapTreeView::onRemoveAttribute, m_TableModel, &CLdapAttributesModel::onRemoveAttribute);
    connect(m_LdapTree, &CLdapTreeView::onAddAttribute, m_TableModel, &CLdapAttributesModel::onAddAttribute);
    connect(m_LdapTree, &CLdapTreeView::manageUsersInGroup, m_AttributesList, &CLdapTableView::onManageUsersInGroup);

    connect(this, &MainWindow::removeEntity, m_TreeModel, &CLdapTreeModel::onRemoveEntity);

    // enable/disable Save button
    connect(m_LdapTree, &CLdapTreeView::entityClassesChanged, this, &MainWindow::onEntityClassesChanged);
    connect(m_TableModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onEntityClassesChanged);
    connect(m_TableModel, &QAbstractItemModel::rowsRemoved, this, &MainWindow::onEntityClassesChanged);
    connect(m_TableModel, &QAbstractItemModel::dataChanged, this, &MainWindow::onEntityClassesChanged);

}



MainWindow::~MainWindow()
{

}

void MainWindow::CreateDockWindows()
{
	QDockWidget* dock = new QDockWidget(tr("LDAP Tree"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	m_LdapTree = new CLdapTreeView(dock, m_LdapData);
	connect(m_LdapTree, &CLdapTreeView::treeItemChanged, this, &MainWindow::onTreeItemChanged);

	m_LdapTree->setHeaderHidden(true);
	dock->setWidget(m_LdapTree);
	addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::CreateActions()
{
	QMenu* dataMenu = menuBar()->addMenu(tr("&Data"));
    QAction* searchAction = dataMenu->addAction(QIcon(":/search"),tr("&Ldap search"), this, &MainWindow::onLdapSearch);
	dataMenu->setStatusTip(tr("Ldap search"));
	dataMenu->addAction(searchAction);

    m_saveDataAction = dataMenu->addAction(QIcon(":/save"),tr("&Save data"), this, &MainWindow::onSaveData);
	dataMenu->setStatusTip(tr("Save data"));
    m_saveDataAction->setEnabled(false);
    dataMenu->addAction(m_saveDataAction);

    m_revertChangesAction = dataMenu->addAction(tr("&Revert changes"), this, &MainWindow::onRevertChanges);
    dataMenu->setStatusTip(tr("Revert pending changes"));
    m_revertChangesAction->setEnabled(m_saveDataAction->isEnabled());
    dataMenu->addAction(m_revertChangesAction);

    dataMenu->addSeparator();
    QAction* reloadDataAction = dataMenu->addAction(QIcon(":/network"),tr("&Reconnect"), this, &MainWindow::onReconnect);
    dataMenu->setStatusTip(tr("Reconnect to server"));
    dataMenu->addAction(reloadDataAction);

    dataMenu->addSeparator();
    QAction* refreshDataAction = dataMenu->addAction(QIcon(":/reload"),tr("Re&fresh"), this, &MainWindow::onReload);
    dataMenu->setStatusTip(tr("Refresh data"));
    dataMenu->addAction(refreshDataAction);

    dataMenu->addSeparator();
	QAction* quitAction = dataMenu->addAction(tr("&Quit"), this, &MainWindow::onQuit);
	dataMenu->setStatusTip(tr("Quit"));
	dataMenu->addAction(quitAction);

	QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
	QAction* aboutAction = helpMenu->addAction(tr("About &App"), this, &MainWindow::onAboutApp);
	aboutAction->setStatusTip(tr("Show the application's About box"));
	helpMenu->addAction(aboutAction);

	QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
	aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
	helpMenu->addAction(aboutQtAction);


    QToolBar* mainToolbar = addToolBar(tr("mainToolbar"));
    mainToolbar->addAction(searchAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(m_saveDataAction);
    mainToolbar->addAction(m_revertChangesAction);
    mainToolbar->addSeparator();
    mainToolbar->addAction(reloadDataAction);
    mainToolbar->addAction(refreshDataAction);
}

void MainWindow::CreateStatusBar()
{
	QString ipStr = QString("%1:%2").arg(m_LdapData.host()).arg(m_LdapData.port());
	m_IpStatus = new QLabel(ipStr);
	statusBar()->addPermanentWidget(m_IpStatus);
	statusBar()->showMessage(tr("Ready"));
}


void MainWindow::onAboutApp()
{
	QString title(tr("About application"));
	QStringList lines;
    lines << QString(tr("%1 application")).arg(ApplicationName);
    lines << QString(tr("Version: %1")).arg(ApplicationVersion);
    lines << QString(tr("License: %1")).arg(tr("GPL v2"));
    lines << QString(tr("© %1, 2018-%2")).arg(OrganizationName).arg(QDate::currentDate().year());
	lines << QString();
    lines << QString(tr("Credentials:"));
    lines << QString(tr("SimpleCrypt, kindly provided by"));
    lines << QString(tr("https://wiki.qt.io/Simple_encryption_with_SimpleCrypt"));
    lines << QString(tr("Icons, are kindly provided by www.flaticon.com"));

	QString text(lines.join("\n"));
	QMessageBox dlg(QMessageBox::Information, title, text, QMessageBox::Ok, this);
	dlg.exec();
}



void MainWindow::UpdateActionStatus()
{

}

QString MainWindow::normilizeDN(const QString& dn)
{
	QStringList resList;
	QStringList parts = dn.split(",");
	if (parts.size() > 1)
	{
		for (const auto& p : parts)
		{
			QStringList kv = p.split("=");
			resList << QString("%1=%2").arg(kv[0].trimmed()).arg(kv[1].trimmed());
		}
		return resList.join(", ");
	}
	return dn.trimmed();
};

void MainWindow::onTreeItemChanged(const QModelIndex& current, const QModelIndex& mainPrev)
{
    if(m_bRevertChangesInProgress) return;

    QModelIndex srcCur = m_TreeProxyModel->mapToSource(current);
    QModelIndex srcPrev = m_TreeProxyModel->mapToSource(mainPrev);
    if (!srcCur.isValid())
	{
		return;
	}

    if (srcPrev.isValid())
	{
        if (m_TableModel->isNew() && m_LdapTree->updatesEnabled())
		{
            auto ret = QMessageBox::question(this, tr("Question"),
                                             tr("The new entry was added.\nDo you want to save new entry to server?"),
                                             QMessageBox::Yes | QMessageBox::No);
            if (ret == QMessageBox::Yes)
            {
                if (!m_TableModel->Save())
                {
                    return;
                }
            }
            else
			{
                emit removeEntity(srcPrev);
            }
		}
		else
		{
            QVector<ldapcore::CLdapAttribute> newRows, deleteRows;
            QMap<QString, QVector<ldapcore::CLdapAttribute>> updateMap;
            m_TableModel->changedRows(newRows, deleteRows, updateMap);
            // first check
            bool hasChanges = !(newRows.empty() && deleteRows.empty() && updateMap.empty());
            // second check
            hasChanges |= m_TableModel->isEdit();
            if (hasChanges && m_LdapTree->updatesEnabled())
			{
                const QString s1(tr("You have changes in attributes.\nDo you want to save these changes to server?"));
                const QString s2(tr("The entry was updated.\nDo you want to save edited entry to server?"));
                auto ret = QMessageBox::question(this, tr("Question"),
                                                 m_TableModel->isEdit() ? s2 : s1,
                                                 QMessageBox::Yes | QMessageBox::No);
				if (ret == QMessageBox::Yes)
				{
					m_TableModel->Save();
				}
                else
                {
                    ldapcore::CLdapEntry* prevEntry = static_cast<ldapcore::CLdapEntry*>(srcPrev.internalPointer());
                    QStringList cls;
                    prevEntry->setClasses(cls);
                    prevEntry->flushAttributesCache();
                    (void*)prevEntry->attributes();
                    m_LdapTree->update(mainPrev);
                }
			}
		}
	}

    ldapcore::CLdapEntry* currentEntry = static_cast<ldapcore::CLdapEntry*>(srcCur.internalPointer());
	if (currentEntry)
	{
        currentEntry->flushAttributesCache();
		m_TableModel->setLdapEntry(currentEntry);
		m_AttributesList->setLdapEntry(currentEntry);

        m_saveDataAction->setEnabled(false);
	}
}

void MainWindow::onLdapSearch()
{
	CLDapSearchDialog searchDlg(m_LdapData, this);
	searchDlg.exec();
}

void MainWindow::onSaveData()
{
    if(m_AttributesList->SaveData())
    {
        statusBar()->showMessage(tr("Data saved"));
    }
}

void MainWindow::onQuit()
{
	close();
}


void MainWindow::onReconnect()
{
    try
    {
        m_LdapData.reconnect();
        QMessageBox::information(this, tr("Reconnect operation"), tr("The operation has been completed successfully!"), QMessageBox::Ok);
    }
    catch (const std::exception& ex)
    {
        QMessageBox::critical(nullptr, tr("Error"), ex.what(), QMessageBox::Ok);
    }
}


void MainWindow::onReload()
{
    onSaveData();

    //m_LdapTree->saveState();

    m_TableModel->setLdapEntry(nullptr);
    m_AttributesList->setLdapEntry(nullptr);

    m_LdapData.rebuild();
    m_TreeModel->setTopItems(m_LdapData.topList());

    m_RootIndex = m_TreeProxyModel->index(0, 0);
    m_LdapTree->expand(m_RootIndex);
    m_LdapTree->setCurrentIndex(m_RootIndex);

    //m_LdapTree->restoreState();

    statusBar()->showMessage(tr("Data reloaded"));
}

void MainWindow::onEntityClassesChanged()
{
    if(!m_LdapTree->updatesEnabled()) return;

    bool enableSave{false};
    if (m_TableModel->isNew())
    {
        enableSave = true;
    }
    else
    {
        QVector<ldapcore::CLdapAttribute> newRows, deleteRows;
        QMap<QString, QVector<ldapcore::CLdapAttribute>> updateMap;
        m_TableModel->changedRows(newRows, deleteRows, updateMap);

        // first check
        bool hasChanges = !(newRows.empty() && deleteRows.empty() && updateMap.empty());

        // second check
        hasChanges |= m_TableModel->isEdit();

        enableSave = hasChanges;

    }
    m_saveDataAction->setEnabled(enableSave);
    m_revertChangesAction->setEnabled(enableSave);
    return;
}

void MainWindow::onRevertChanges()
{
    QModelIndex idx = m_LdapTree->currentIndex();
    if(!idx.isValid()) return;

    QModelIndex srcIdx = m_TreeProxyModel->mapToSource(idx);
    if(!srcIdx.isValid()) return;


    ldapcore::CLdapEntry* entry = static_cast<ldapcore::CLdapEntry*>(srcIdx.internalPointer());
    ldapcore::CLdapEntry* parentEntry = entry->parent();
    if (m_TableModel->isNew())
    {
        common::CBooleanAutoFlag f(m_bRevertChangesInProgress, true);
        m_TreeModel->revert();
        m_TreeModel->removeRow(srcIdx.row(), srcIdx.parent());
        m_LdapTree->setCurrentIndex(idx.parent());
        parentEntry->removeChild(entry);

        m_TableModel->setLdapEntry(nullptr);
        m_AttributesList->setLdapEntry(nullptr);

    }
    else if(m_TableModel->isEdit())
    {
        common::CBooleanAutoFlag f(m_bRevertChangesInProgress, true);
        if(entry)
        {
            entry->setEditable(false);

            QStringList cls;
            entry->setClasses(cls);
            entry->flushAttributesCache();
            (void*)entry->attributes();
            m_LdapTree->update(idx);
            m_LdapTree->setCurrentIndex(idx);

            m_TableModel->setLdapEntry(entry);
           m_AttributesList->setLdapEntry(entry);
        }
    }
    else
    {
        m_TableModel->revert();
        m_LdapTree->setCurrentIndex(idx);
        m_TableModel->setLdapEntry(entry);
        m_AttributesList->setLdapEntry(entry);
    }
}

}// namespace ldapeditor

