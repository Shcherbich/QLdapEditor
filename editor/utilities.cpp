/*!
\file
\brief Implementation for utility functions

File contains  implementations for utility functions
*/

#include "utilities.h"
namespace ldapeditor {

QString userNameFromDN(const QString dn)
{
    QStringList parts = dn.split(",");
    if(parts.isEmpty()) return "";

    const QString& first = parts.first();
    return first.mid(first.lastIndexOf('=')+1).trimmed();
}

void moveItemBettwenLists(QListWidget* sourceList, QListWidget* destList)
{
    if(sourceList && destList)
    {
        for(auto i: sourceList->selectedItems())
        {
            QListWidgetItem* cloneItem = i->clone();
            destList->addItem(cloneItem);
        }
        qDeleteAll(sourceList->selectedItems());
    }
}

} //namespace ldapeditor

