#ifndef CLDAPTREEMODEL_H
#define CLDAPTREEMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QVector>
#include <QString>

namespace ldapcore {
class CLdapEntry;
class CLdapAttribute;
}

namespace ldapeditor
{

    class CLdapTreeProxyModel : public QSortFilterProxyModel
    {
        Q_OBJECT
    public:
        explicit CLdapTreeProxyModel(QObject *parent = nullptr):QSortFilterProxyModel(parent)
        {
        }
    protected:
        virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    };

    class CLdapTreeModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        explicit CLdapTreeModel(const QString& baseDN, QObject *parent = nullptr);
        bool setTopItems(QVector<ldapcore::CLdapEntry*> topItems);

        // Basic functionality:
        QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex &index) const override;

        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;

        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex &index, const QVariant &value,
                     int role = Qt::EditRole) override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;

        Q_INVOKABLE void removeAttribute(QString name);
        Q_INVOKABLE void addAttribute(QString name);

        QModelIndex addNewEntry(QModelIndex parent, QString rdn, QString dn, QVector<QString>& classes, QVector<ldapcore::CLdapAttribute>& attributes);

    private:
        ldapcore::CLdapEntry*           m_invisibleRoot{nullptr};
        QVector<ldapcore::CLdapEntry*>  m_topItems;

    Q_SIGNALS:
        void onRemovingAttribute(QString name);
        void onAddAttribute(QString name);

    };
} //namespace ldapeditor


#endif // CLDAPTREEMODEL_H
