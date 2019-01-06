#ifndef LDAPTABLEVIEW_H
#define LDAPTABLEVIEW_H

#include <QTableView>
#include <QSortFilterProxyModel>
namespace ldapeditor
{
    /*
    class CTableFilterProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT

    public:
        CTableFilterProxyModel(QObject *parent = nullptr):QSortFilterProxyModel(parent)
         , m_Sections{ tr("Dn"), tr("Attribute"), tr("Value"), tr("Type"), tr("Size") }
        {
            setDynamicSortFilter(true);
            setFilterKeyColumn(0);
            setRecursiveFilteringEnabled(true);
        }
        inline void updateFilter(){invalidateFilter();}

        virtual QVariant
        headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;


        virtual int
        columnCount(const QModelIndex &parent = QModelIndex()) const override{
            Q_UNUSED(parent)
            return m_Sections.size();
        }

     protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
        QStringList m_Sections;
    };
    */

    class CLdapTableView : public QTableView
    {
        Q_OBJECT
    public:
        explicit CLdapTableView(QWidget *parent = nullptr);

    signals:

    public slots:
    };

} //namespace ldapeditor
#endif // LDAPTABLEVIEW_H
