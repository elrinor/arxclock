#ifndef __ARXCLOCK_MAIN_DIALOG_H__
#define __ARXCLOCK_MAIN_DIALOG_H__

#include "config.h"
#include <QtGui>

class MainDialog: public QDialog {
  Q_OBJECT;

public:
  MainDialog(QSettings* settings);

protected:
  virtual void closeEvent(QCloseEvent *event);
  virtual bool eventFilter(QObject* sender, QEvent* e);

private slots:
  void addClicked();
  void editClicked();
  void deleteClicked();
  void clicked(const QModelIndex&);

private:
  QSettings* settings;
  QTreeView *treeView;
  QStandardItemModel* itemModel;
};

#endif // __ARXCLOCK_MAIN_DIALOG_H__
