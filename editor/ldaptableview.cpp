/*!
@file
@brief Implementation file for LDAP attributes view class

File contains  implementations for LDAP attributes view class
*/
#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"
#include <QHeaderView>

#include "ldapattributesmodel.h"
#include "ldapnewattributedialog.h"
#include "addusertogroupdialog.h"

namespace ldapeditor
{
    CLdapTableView::CLdapTableView(QWidget *parent, ldapcore::CLdapData& ldapData, CLdapSettings& s)
        : QTableView(parent), m_LdapData(ldapData), m_LdapSettings(s)
    , m_ldapDataDelegate(this)
    , m_defaultDelegate(this)
    , m_contextMenu(this)
    , m_newAttr(new QAction(tr("New attribute"), this))
    , m_delAttr(new QAction(tr("Delete attribute"), this))
    , m_addUser2Group(new QAction(tr("Manage users in group"), this))
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setAlternatingRowColors(true);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSortingEnabled(false);

        connect(m_newAttr, &QAction::triggered, this, &CLdapTableView::onNewAttribute);
        connect(m_delAttr, &QAction::triggered, this, &CLdapTableView::onDeleteAttribute);
        connect(m_addUser2Group, &QAction::triggered, this, &CLdapTableView::onAddUserToGroup);

        m_contextMenu.addAction(m_newAttr);
        m_contextMenu.addAction(m_delAttr);
        m_contextMenu.addAction(m_addUser2Group);

        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QTableView::customContextMenuRequested, this, &CLdapTableView::customContextMenuRequested);
    }

    void CLdapTableView::setLdapEntry(ldapcore::CLdapEntry* entry)
    {
        m_entry = entry;
        m_ldapDataDelegate.setLdapEntry(m_entry);

        if(m_entry && !m_entry->isNew())
            hideColumn(static_cast<int>(AttributeColumn::Ignore));
        else
            showColumn(static_cast<int>(AttributeColumn::Ignore));
    }

    void CLdapTableView::RestoreView()
    {
        setColumnWidth(static_cast<int>(AttributeColumn::Ignore), m_LdapSettings.columnIgnore());
        setColumnWidth(static_cast<int>(AttributeColumn::Name), m_LdapSettings.columnDn());
        setColumnWidth(static_cast<int>(AttributeColumn::Class), m_LdapSettings.columnClass());
        setColumnWidth(static_cast<int>(AttributeColumn::Attribute), m_LdapSettings.columnAttribute());
        setColumnWidth(static_cast<int>(AttributeColumn::Value), m_LdapSettings.columnValue());
        setColumnWidth(static_cast<int>(AttributeColumn::Type), m_LdapSettings.columnType());
        setColumnWidth(static_cast<int>(AttributeColumn::Size), m_LdapSettings.columnSize());

        hideColumn(static_cast<int>(AttributeColumn::Name));
        hideColumn(static_cast<int>(AttributeColumn::Size));

        connect(horizontalHeader(), &QHeaderView::sectionResized, this, &CLdapTableView::OnHeaderChanged);
    }

    bool CLdapTableView::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)
    {
        if(index.column() == static_cast<int>(AttributeColumn::Attribute))
        {
            setItemDelegate(&m_ldapDataDelegate);
        }
        else if(index.column() == static_cast<int>(AttributeColumn::Value))
        {
            ldapcore::AttrType type = static_cast<ldapcore::AttrType>(index.data(ldapeditor::AttrTypeRole).toInt());
            switch(type)
            {
            case ldapcore::AttrType::Binary:
            case ldapcore::AttrType::TelephoneNumber:
            case ldapcore::AttrType::GeneralizedTime:
                setItemDelegate(&m_ldapDataDelegate);
                break;
            default:
                setItemDelegate(&m_defaultDelegate);
            }
        }
        else if(index.column() == static_cast<int>(AttributeColumn::Type))
        {
            setItemDelegate(&m_ldapDataDelegate);
        }
        return QTableView::edit(index, trigger, event);
    }

    void CLdapTableView::OnHeaderChanged(int logicalIndex, int oldSize, int newSize)
    {
        Q_UNUSED(logicalIndex);
        Q_UNUSED(oldSize);
        Q_UNUSED(newSize);
        m_LdapSettings.setColumnIgnore(columnWidth(static_cast<int>(AttributeColumn::Ignore)));
        m_LdapSettings.setColumnDn(columnWidth(static_cast<int>(AttributeColumn::Name)));
        m_LdapSettings.setColumnAttribute(columnWidth(static_cast<int>(AttributeColumn::Attribute)));
        m_LdapSettings.setColumnValue(columnWidth(static_cast<int>(AttributeColumn::Value)));
        m_LdapSettings.setColumnType(columnWidth(static_cast<int>(AttributeColumn::Type)));
        m_LdapSettings.setColumnSize(columnWidth(static_cast<int>(AttributeColumn::Size)));
       // m_LdapSettings.sync();
    }

    void CLdapTableView::customContextMenuRequested(QPoint pos)
    {
        const QStringList exludeAttributes{"member","memberOf"};
        QModelIndex index = indexAt(pos);


        bool delEnable{false};
        if(index.isValid())
        {
            // attribute can be deleted and Not ('member' or 'memberOf')'
            delEnable = m_ldapDataDelegate.canDeleteRow(index) &&
                        !exludeAttributes.contains(index.data(ldapeditor::LDapEditorRoles::AttrNameRole).toString());
        }
        m_delAttr->setEnabled(delEnable);
        m_delAttr->setData(index);

        QStringList classes = m_entry->classes();
        bool add2GroupVisible = std::find_if(classes.begin(), classes.end(), [](const QString& c){
                                             return c.compare("group", Qt::CaseInsensitive) == 0;
                                 })!= classes.end();
        m_addUser2Group->setVisible(add2GroupVisible);

       m_contextMenu.popup(viewport()->mapToGlobal(pos));
    }

    void CLdapTableView::onNewAttribute()
    {
        ldapeditor::CLdapNewAttributeDialog dlg(m_LdapData, m_entry);
        if(dlg.exec() == QDialog::Accepted)
        {
            QModelIndex newAttr = static_cast<CLdapAttributesModel*>(model())->addAttribute(dlg.attribute());
            if(newAttr.isValid())
            {
                setCurrentIndex(newAttr);
            }
        }

    }

    void CLdapTableView::onDeleteAttribute()
    {
        QAction* a = qobject_cast<QAction*>(sender());
        QModelIndex i = a->data().toModelIndex();
        if(i.isValid())
        {
            model()->removeRows(i.row(), 1, i.parent());
        }
    }

    void CLdapTableView::mousePressEvent(QMouseEvent *event)
    {
        QModelIndex index = indexAt(event->pos());
        if(index.isValid())
        {
            if(index.column() == static_cast<int>(AttributeColumn::Ignore))
            {
                bool isChecked = index.data(Qt::CheckStateRole).toInt() == Qt::Checked;
                model()->setData(index, QVariant(!isChecked), Qt::CheckStateRole );
                event->accept();
            }
        }
        return QTableView::mousePressEvent(event);
    }

    bool CLdapTableView::SaveData()
    {
        bool isNew = m_entry->isNew();
        bool bRet = static_cast<CLdapAttributesModel*>(model())->Save();
        if(isNew)
        {
            hideColumn(static_cast<int>(AttributeColumn::Ignore));
        }
        return bRet;
    }

    void CLdapTableView::onAddUserToGroup()
    {
        QStringList originalMembers;
        const QVector<ldapcore::CLdapAttribute>* attrs = m_entry->attributes();
        if(attrs)
        {
            std::for_each(attrs->begin(), attrs->end(),[&originalMembers](const ldapcore::CLdapAttribute& a) {
                if(a.name().compare("member", Qt::CaseInsensitive) == 0)
                    originalMembers << a.value();
            });
        }

        CAddUserToGroupDialog dlg(m_LdapData, m_LdapSettings);
        dlg.setMembersList(originalMembers);
        if(dlg.exec() != QDialog::Accepted)
        {
            return;
        }

        QStringList newMembers = dlg.membersList();
        QStringList classes {"group"};//= m_entry->classes();
        CLdapAttributesModel* srcModel = static_cast<CLdapAttributesModel*>(model());

        // add appended members attributes
        for(const QString& s : newMembers)
        {
            if(srcModel && !originalMembers.contains(s))
            {

                ldapcore::CLdapAttribute newAttr;
                newAttr.setEditState(ldapcore::AttributeState::AttributeReadWrite);
                newAttr.setClasses(classes);
                newAttr.setName("member");
                newAttr.setValue(s);
                newAttr.setType(ldapcore::AttrType::DN);
                newAttr.setEditState(ldapcore::AttributeState::AttributeReadOnly);
                (void) srcModel->addAttribute(newAttr);
            }
        }

        // delete removed members attributes
        QModelIndexList indexes;
        for(const QString& s : originalMembers)
        {
            if(srcModel && !newMembers.contains(s))
            {
              // find index by value
               indexes = srcModel->match(srcModel->index(0,static_cast<int>(ldapeditor::AttributeColumn::Value)), Qt::DisplayRole, s, 1, Qt::MatchExactly);
               for(QModelIndex idx : indexes)
               {
                   srcModel->removeRows(idx.row(),1);
               }
            }
        }
    }

} //namespace ldapeditor
