/*!
@file
@brief Implementation file for LDAP attributes view class

File contains  implementations for LDAP attributes view class
*/
#include "ldaptableview.h"
#include "ldapeditordefines.h"
#include "CLdapAttribute.h"
#include <QHeaderView>
#include <QLineEdit>

#include "ldapattributesmodel.h"
#include "ldapnewattributedialog.h"
#include "addusertogroupdialog.h"

namespace ldapeditor
{
    CAttributesList::CAttributesList(QWidget *parent, ldapcore::CLdapData& ldapData, CLdapSettings& s)
        : QTableView(parent), m_LdapData(ldapData), m_LdapSettings(s)
    , m_ldapDataDelegate(this)
    , m_defaultDelegate(this)
    , m_contextMenu(this)
    , m_newAttr(new QAction(tr("New attribute"), this))
    , m_delAttr(new QAction(tr("Delete attribute"), this))
    {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setAlternatingRowColors(true);
        setSelectionMode(QAbstractItemView::SingleSelection);
        setSelectionBehavior(QAbstractItemView::SelectRows);
        setSortingEnabled(false);

        connect(m_newAttr, &QAction::triggered, this, &CAttributesList::onNewAttribute);
        connect(m_delAttr, &QAction::triggered, this, &CAttributesList::onDeleteAttribute);

        m_contextMenu.addAction(m_newAttr);
        m_contextMenu.addAction(m_delAttr);

        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &QTableView::customContextMenuRequested, this, &CAttributesList::customContextMenuRequested);
    }

    void CAttributesList::setLdapEntry(ldapcore::CLdapEntry* entry)
    {
        m_entry = entry;
        m_ldapDataDelegate.setLdapEntry(m_entry);

        if(m_entry && !m_entry->isNew())
            hideColumn(static_cast<int>(AttributeColumn::Ignore));
        else
            showColumn(static_cast<int>(AttributeColumn::Ignore));
    }

    void CAttributesList::RestoreView()
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

        connect(horizontalHeader(), &QHeaderView::sectionResized, this, &CAttributesList::OnHeaderChanged);
    }

    bool CAttributesList::edit(const QModelIndex& index, QAbstractItemView::EditTrigger trigger, QEvent* event)
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

    void CAttributesList::OnHeaderChanged(int logicalIndex, int oldSize, int newSize)
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

    void CAttributesList::customContextMenuRequested(QPoint pos)
    {
        const QStringList exludeAttributes{"member","memberOf"};
        QModelIndex index = indexAt(pos);

        bool delEnable{false};
        bool isRootAttributes = m_entry ? !m_entry->parent() : true;
        if(index.isValid())
        {
            // attribute can be deleted and Not ('member' or 'memberOf')'
            delEnable = m_ldapDataDelegate.canDeleteRow(index) &&
                        !exludeAttributes.contains(index.data(ldapeditor::LDapEditorRoles::AttrNameRole).toString());
        }
        m_delAttr->setEnabled(delEnable);
        m_delAttr->setData(index);

        if(!isRootAttributes)
            m_contextMenu.popup(viewport()->mapToGlobal(pos));
    }

    void CAttributesList::onNewAttribute()
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

    void CAttributesList::onDeleteAttribute()
    {
        QAction* a = qobject_cast<QAction*>(sender());
        QModelIndex i = a->data().toModelIndex();
        if(i.isValid())
        {
            model()->removeRows(i.row(), 1, i.parent());
        }
    }

    void CAttributesList::mousePressEvent(QMouseEvent *event)
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

    bool CAttributesList::SaveData()
    {
        bool isNew = m_entry->isNew();
        bool bRet = static_cast<CLdapAttributesModel*>(model())->Save();
        if(isNew)
        {
            hideColumn(static_cast<int>(AttributeColumn::Ignore));
        }
        return bRet;
    }

    void CAttributesList::onManageUsersInGroup()
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
        QStringList classes {"group"};
        CLdapAttributesProxyModel* proxyModel = static_cast<CLdapAttributesProxyModel*>(model());
        CLdapAttributesModel* srcModel = static_cast<CLdapAttributesModel*>(proxyModel->sourceModel());

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
               indexes = srcModel->match(srcModel->index(0, static_cast<int>(ldapeditor::AttributeColumn::Value)),
                                                          Qt::DisplayRole, s, 1, Qt::MatchExactly);
               for(QModelIndex idx : indexes)
               {
                   srcModel->removeRows(idx.row(),1);
               }
            }
        }
    }


    CLdapTableView::CLdapTableView(QWidget *parent, ldapcore::CLdapData& ldapData, CLdapSettings& s)
        : QWidget(parent)
        , m_layout(new QVBoxLayout)
        , m_filterEdit(new QLineEdit)
        , m_attrList(new CAttributesList(this, ldapData, s))
        , m_proxyModel(new CLdapAttributesProxyModel(this))
    {
        m_attrList->horizontalHeader()->setDefaultSectionSize(100);
        m_attrList->horizontalHeader()->setStretchLastSection(true);
       // m_attrList->horizontalHeader()->setSortIndicatorShown(true);

        m_attrList->setSortingEnabled(true);
        m_attrList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

//        connect(m_attrList->horizontalHeader(), &QHeaderView::sortIndicatorChanged, [this](int logicalIndex, Qt::SortOrder order){
//         //  this->m_attrList->sortByColumn(logicalIndex, order);
//        });

        m_filterEdit->setPlaceholderText("Attribute name/value filter");
        m_filterEdit->setClearButtonEnabled(true);
        connect(m_filterEdit, &QLineEdit::textEdited, [this](QString text){
            if(this->m_proxyModel)
            {
                this->m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
                this->m_proxyModel->setFilterRole(Qt::DisplayRole);
                this->m_proxyModel->setFilterRegExp(text);
            }
        });

        m_layout->addWidget(m_filterEdit);
        m_layout->addWidget(m_attrList);
        setLayout(m_layout);
    }

     void CLdapTableView::setModel(QAbstractItemModel *model)
     {
         m_proxyModel->setSourceModel(model);
         m_attrList->setModel(m_proxyModel);
     //    m_proxyModel->sort(static_cast<int>(ldapeditor::AttributeColumn::Attribute), Qt::AscendingOrder);
     //    m_attrList->horizontalHeader()->setSortIndicator(static_cast<int>(ldapeditor::AttributeColumn::Attribute), Qt::AscendingOrder);
         m_attrList->sortByColumn(static_cast<int>(ldapeditor::AttributeColumn::Attribute),Qt::AscendingOrder);


     }

    void CLdapTableView::setLdapEntry(ldapcore::CLdapEntry* entry)
    {
        m_attrList->setLdapEntry(entry);
    }

    void CLdapTableView::RestoreView()
    {
        m_attrList->RestoreView();
    }
    bool CLdapTableView::SaveData()
    {
        return m_attrList->SaveData();
    }

    void CLdapTableView::onManageUsersInGroup()
    {
        m_attrList->onManageUsersInGroup();
    }
} //namespace ldapeditor
