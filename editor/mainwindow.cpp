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
        setMinimumSize(800,600);

        CreateDockWindows();
        CreateActions();
        CreateStatusBar();

        QString baseDN = normilizeDN(m_Settings.baseDN());
        QString host = m_Settings.host();

        m_TreeModel = new CLdapTreeModel(baseDN, this);
        m_TableModel = new CLdapAttributesModel(m_uniqueDNs, this);
        m_AttributesList = new CLdapTableView(this);

        setCentralWidget(m_AttributesList);
        m_AttributesList->horizontalHeader()->setDefaultSectionSize(width() / m_TableModel->columnCount());

        for(auto e : m_LdapData.topList())
        {
            m_TreeModel->addLdapItem(e->rDn());
        }
//        m_TreeModel->addLdapItem("ou=mathematicians,dc=example,dc=com");
//        m_TreeModel->addLdapItem("uid=riemann,ou=mathematicians,dc=example,dc=com");
//        m_TreeModel->addLdapItem("uid=gauss,ou=mathematicians,dc=example,dc=com");
//        m_TreeModel->addLdapItem("uid=euler,dc=example,dc=com");
//        m_TreeModel->addLdapItem("uid=euclid,dc=example,dc=com");
        m_RootIndex = m_TreeModel->index(0,0);

        m_TableModel->setBaseDN(baseDN);
        m_AttributesList->setModel(m_TableModel);
        m_AttributesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

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
        QMenu *searchMenu = menuBar()->addMenu(tr("&Search"));
        QAction *searchAction = searchMenu->addAction(tr("&Ldap search"), this, &MainWindow::onLdapSearch);
        searchAction->setStatusTip(tr("Ldap search"));
        searchMenu->addAction(searchAction);

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
        QMessageBox dlg(QMessageBox::Information, title, text, QMessageBox::Ok);
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

    QStandardItem* MainWindow::prepareRootItem(const QString& dn, const QString& host)
    {
        QString ndn=normilizeDN(dn);
        QString title(ndn);
        title += QString(" (%1)").arg(host);
        QStringList parts = ndn.split(",");
        QStandardItem* item =  new QStandardItem(title);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        if(parts.length() > 1 )
        {
            item->setData(parts, ldapeditor::AttributesListRole);
        }

        return item;
    }

    QStandardItem* MainWindow::prepareDataForDn(const QString& dn, QStandardItem* parentItem)
    {
        QString ndn=normilizeDN(dn);
        QStringList parts = ndn.split(",");
        QStandardItem* item =  new QStandardItem(ndn);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

        if(parts.length() > 1 )
        {
            item->setData(parts, ldapeditor::AttributesListRole);
        }
        parentItem->appendRow(item);
        return item;
    }

    void MainWindow::onTreeItemChanged(const QModelIndex& current, const QModelIndex& previous)
    {
        if(previous.isValid() && m_TableModel->IsChanged())
        {
            QStringList l = m_TableModel->attributesList();
            QString dn = l.join(", ");
            m_TreeModel->setData(previous, l, ldapeditor::AttributesListRole);
            m_TreeModel->setData(previous, dn, Qt::DisplayRole);
        }
         m_TableModel->setAttributesList(current.data(ldapeditor::AttributesListRole).toStringList());
    }

    void MainWindow::onLdapSearch()
    {
        CLDapSearchDialog searchDlg(m_LdapData);
        searchDlg.exec();
    }
}// namespace ldapeditor
