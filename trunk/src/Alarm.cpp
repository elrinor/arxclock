#include <exception>
#include "Alarm.h"

QList<QStandardItem*> Alarm::toRow() const {
  QList<QStandardItem*> result; 
  
  QStandardItem* enabledItem = new QStandardItem();
  enabledItem->setData(this->mId, Qt::UserRole);
  enabledItem->setData(this->mEnabled ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
  enabledItem->setEditable(false);

  QStandardItem* msgItem = new QStandardItem(this->mMessage);
  msgItem->setData(this->mId, Qt::UserRole);
  msgItem->setEditable(false);
  
  QStandardItem* typeItem = new QStandardItem(this->toString());
  typeItem->setData(this->mId, Qt::UserRole);
  typeItem->setEditable(false);

  return result << enabledItem << msgItem << typeItem;
}
