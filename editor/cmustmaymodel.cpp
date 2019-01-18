#include <QColor>
#include "cmustmaymodel.h"

namespace ldapeditor
{

   MustMayModel::MustMayModel(QObject* parent, ldapcore::CLdapEntry* entry)
       : QAbstractListModel(parent)
   {
       must = entry->availableAttributesMust();
       may = entry->availableAttributesMay();
   }

   int MustMayModel::rowCount(const QModelIndex &parent) const
   {
       return must.size() + may.size();
   }

   QVariant MustMayModel::data(const QModelIndex &index, int role) const
   {
      if (!index.isValid())
          return QVariant();
      int row = index.row();
      if (role == Qt::TextColorRole)
      {
          if (row < must.size() && must.size())
              return QColor(Qt::blue);
          return QColor(Qt::black);
      }

      if (role == Qt::DisplayRole)
      {
          if (row < must.size() && must.size())
              return must[row].name();
          return may[row - must.size()].name();
      }
      return QVariant();
   }

}
