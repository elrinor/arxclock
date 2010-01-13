#ifndef __ARXCLOCK_MAIN_DIALOG_H__
#define __ARXCLOCK_MAIN_DIALOG_H__

#include "config.h"
#include <QtGui>
#include "AlarmManager.h"

// -------------------------------------------------------------------------- //
// MainWidgetKeys
// -------------------------------------------------------------------------- //
class MainWidgetKeys {
public:
  static QString keySize() { return "main_widget/size"; }
  static QString keyPos () { return "main_widget/position"; }
};


// -------------------------------------------------------------------------- //
// MainWidget
// -------------------------------------------------------------------------- //
class MainWidget: public QWidget, private MainWidgetKeys {
  Q_OBJECT;

public:
  MainWidget(QSettings* settings, AlarmManager* alarmManager);

  void updateNextRunTimes();

protected:
  virtual void closeEvent(QCloseEvent *event);
  virtual bool eventFilter(QObject* sender, QEvent* e);

private slots:
  void addClicked();
  void editClicked();
  void deleteClicked();
  void clicked(const QModelIndex&);

private:
  static QList<QStandardItem*> alarmToRow(boost::shared_ptr<Alarm> alarm);
  static QString alarmNextRun(boost::shared_ptr<Alarm> alarm);
  void toggleEnabledSelected();

  QSettings* mSettings;
  QTreeView* mTreeView;
  QStandardItemModel* mItemModel;
  AlarmManager* mAlarmManager;
};

#endif // __ARXCLOCK_MAIN_DIALOG_H__
