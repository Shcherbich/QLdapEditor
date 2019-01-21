#ifndef CMUSTMAYMODEL_H
#define CMUSTMAYMODEL_H


#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include "CLdapAttribute.h"
#include "CLdapEntry.h"

namespace ldapeditor{

class MustMayModel : public QAbstractListModel
{
   Q_OBJECT
public:
   MustMayModel(QObject* parent, ldapcore::CLdapEntry* entry, QVector<ldapcore::CLdapAttribute>* existAttributes);

   int rowCount(const QModelIndex &parent = QModelIndex()) const;
   QVariant data(const QModelIndex &index, int role) const;

private:
   QVector<ldapcore::CLdapAttribute> must;
   QVector<ldapcore::CLdapAttribute> may;

};

} //namespace ldapeditor

#endif // CMUSTMAYMODEL_H
