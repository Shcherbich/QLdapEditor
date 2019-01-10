#ifndef DATETIMEEDITDELEGATE_H
#define DATETIMEEDITDELEGATE_H

#include <QStyledItemDelegate>
namespace ldapeditor {
    class CDateTimeEditDelegate : public QStyledItemDelegate
    {
        Q_OBJECT
    protected:
        typedef enum class  {
            DateOnly=0,
            TimeOnly,
            DateTime
        } DataType;
       mutable DataType m_type;
    public:
        CDateTimeEditDelegate(QObject *parent = nullptr);
        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const override;

              void setEditorData(QWidget *editor, const QModelIndex &index) const override;
              void setModelData(QWidget *editor, QAbstractItemModel *model,
                                const QModelIndex &index) const override;

              void updateEditorGeometry(QWidget *editor,
                  const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };
} //namespace ldapeditor
#endif // DATETIMEEDITDELEGATE_H
