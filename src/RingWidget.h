#ifndef ARXCLOCK_RING_WIDGET_H
#define ARXCLOCK_RING_WIDGET_H

#include "config.h"
#include <boost/shared_ptr.hpp>
#include <QtGui>
#include "Alarm.h"
#include "MainWidget.h"

// -------------------------------------------------------------------------- //
// RingWidgetKeys
// -------------------------------------------------------------------------- //
class RingWidgetKeys {
public:
  static QString keySize() { return "ring_widget/size"; }
  static QString keyPos () { return "ring_widget/position"; }
};


// -------------------------------------------------------------------------- //
// RingWidget
// -------------------------------------------------------------------------- //
class RingWidget: public QWidget, private RingWidgetKeys {
  Q_OBJECT;

public:
  RingWidget(QSettings* settings, boost::shared_ptr<Alarm> alarm, MainWidget* mainWidget);

protected:
  virtual void closeEvent(QCloseEvent *event);

private slots:
  void snooze5();
  void snooze10();
  void snooze15();
  void snooze30();
  void snooze60();
  void snoozeDialog();
  void more();

private:
  QSettings* mSettings;
  boost::shared_ptr<Alarm> mAlarm;

  int mSnoozeSecs;
  QMenu* mMoreMenu;
  QPushButton* mMoreButton;
  MainWidget* mMainWidget;
};

#endif // ARXCLOCK_RING_WIDGET_H
