#include "mainwindow.h"
#include "ldapeditordefines.h"
#include "ldapsearchdialog.h"
#include "ldaptreemodel.h"
#include "ldapattributesmodel.h"
#include "ldapsettings.h"

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

namespace ldapeditor
{
    MainWindow::MainWindow(CLdapSettings& settings, ldapcore::CLdapData& ldapData, QWidget* parent)
        : QMainWindow(parent)
        , m_Settings(settings)
        , m_LdapData(ldapData)
    {
        setWindowTitle(ApplicationName);
        setMinimumSize(800,600);

        CreateDockWindows();
        CreateActions();
        CreateStatusBar();

        QString baseDN = normilizeDN(m_Settings.baseDN());

        m_TreeModel = new CLdapTreeModel(baseDN, this);
        m_TableModel = new CLdapAttributesModel(this);
        m_AttributesList = new CLdapTableView(this, m_Settings);

        setCentralWidget(m_AttributesList);
        m_AttributesList->horizontalHeader()->setDefaultSectionSize(100);
        m_AttributesList->horizontalHeader()->setStretchLastSection(true);

        m_TreeModel->setTopItems(m_LdapData.topList());
        m_RootIndex = m_TreeModel->index(0,0);

        m_TableModel->setBaseDN(baseDN);
        m_AttributesList->setModel(m_TableModel);
        m_AttributesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_AttributesList->RestoreView();

        m_LdapTree->setModel(m_TreeModel);
        m_LdapTree->header()->resizeSection(0,m_LdapTree->header()->width());
        m_LdapTree->expand(m_RootIndex);
        m_LdapTree->setCurrentIndex(m_RootIndex);
    }


    MainWindow::~MainWindow()
    {

    }

    void MainWindow::CreateDockWindows()
      {
          QDockWidget *dock = new QDockWidget(tr("LDAP Tree"), this);
          dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
          m_LdapTree = new CLdapTreeView(dock);
          connect(m_LdapTree,&CLdapTreeView::treeItemChanged, this, &MainWindow::onTreeItemChanged);

          m_LdapTree->setHeaderHidden(true);
          dock->setWidget(m_LdapTree);
          addDockWidget(Qt::LeftDockWidgetArea, dock);
    }

    void MainWindow::CreateActions()
    {
        QMenu *dataMenu = menuBar()->addMenu(tr("&Data"));
        QAction *searchAction = dataMenu->addAction(tr("&Ldap search"), this, &MainWindow::onLdapSearch);
        dataMenu->setStatusTip(tr("Ldap search"));
        dataMenu->addAction(searchAction);

        QAction *saveDataAction = dataMenu->addAction(tr("&Save data"), this, &MainWindow::onSaveData);
        dataMenu->setStatusTip(tr("Save data"));
        dataMenu->addAction(saveDataAction);

        QAction* quitAction = dataMenu->addAction(tr("&Quit"), this, &MainWindow::onQuit);
        dataMenu->setStatusTip(tr("Quit"));
        dataMenu->addAction(quitAction);

        QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
        QAction *aboutAction = helpMenu->addAction(tr("About &App"), this, &MainWindow::onAboutApp);
        aboutAction->setStatusTip(tr("Show the application's About box"));
        helpMenu->addAction(aboutAction);

        QAction *aboutQtAction = helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
        aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));
        helpMenu->addAction(aboutQtAction);
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
        lines << QString("%1 application").arg(ApplicationName);
        lines << QString("Version: %1").arg(ApplicationVersion);
        lines << QString("License: %1").arg("GPL v2");
        lines << QString("© %1, 2018-%2").arg(OrganizationName).arg(QDate::currentDate().year());
        lines << QString();
        lines << QString("Credentials:");
        lines << QString("SimpleCrypt, kindly provided by");
        lines << QString("https://wiki.qt.io/Simple_encryption_with_SimpleCrypt");
        lines << QString("Icons, are kindly provided by www.flaticon.com");

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
        if(parts.size()>1)
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

    void MainWindow::onTreeItemChanged(const QModelIndex& current, const QModelIndex& previous)
    {
        if(!current.isValid())
        {
            return;
        }

        if(previous.isValid())
        {
            QVector<ldapcore::CLdapAttribute> newRows, deleteRows, updateRows;
            m_TableModel->GetChangedRows(newRows, deleteRows, updateRows);
            bool hasChanges = !(!newRows.size() && !deleteRows.size() && !updateRows.size());
            if (hasChanges)
            {
                auto ret = QMessageBox::question(this, "Question", "You have changes in attributes.\nDo you want to save these changes to server?", QMessageBox::Yes|QMessageBox::No);
                if (ret == QMessageBox::Yes)
                {
                    m_TableModel->SaveToServer();
                }
            }
        }

        ldapcore::CLdapEntry* currentEntry = static_cast<ldapcore::CLdapEntry*>(current.internalPointer());
        if(currentEntry)
        {
            QVector<ldapcore::CLdapAttribute>* pAttrs = currentEntry->attributes();
            m_TableModel->setLdapEntry(currentEntry);
            m_AttributesList->setLdapEntry(currentEntry);
        }
    }

    void MainWindow::onLdapSearch()
    {
        CLDapSearchDialog searchDlg(m_LdapData, this);
        searchDlg.exec();
    }

    void MainWindow::onSaveData()
    {
        m_TableModel->SaveToServer();
    }

    void MainWindow::onQuit()
    {
        close();
    }


    void MainWindow::onReload()
    {

    }
}// namespace ldapeditor

