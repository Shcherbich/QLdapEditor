#include <algorithm>
#include <QColor>
#include "cmustmaymodel.h"

namespace ldapeditor
{

   MustMayModel::MustMayModel(QObject* parent, ldapcore::CLdapEntry* entry, QVector<ldapcore::CLdapAttribute>* existAttributes)
       : QAbstractListModel(parent)
   {
       auto allMays = entry->availableAttributesMay();
       for (const auto& m : allMays)
       {
           auto f = std::find_if(existAttributes->begin(), existAttributes->end(), [&](const ldapcore::CLdapAttribute& a)
           {
              return a.name() == m.name();
           });
           if (f == existAttributes->end())
           {
               may.push_back(m);
           }
       }
       //must = entry->availableAttributesMust();
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
