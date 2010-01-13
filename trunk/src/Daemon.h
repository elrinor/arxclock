#ifndef __ARXCLOCK_DAEMON_H__
#define __ARXCLOCK_DAEMON_H__

#include "config.h"
#include <QtGui>
#include "MainWidget.h"
#include "RingWidget.h"

class Daemon: public QObject {
private:
  Q_OBJECT

public:
  Daemon(QSettings* settings);
  ~Daemon();

private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void timeout();

private:
  QTimer* mTimer;
  QSettings* mSettings;
  QSystemTrayIcon* mTrayIcon;
  MainWidget* mMainWidget;

  AlarmManager mAlarmManager;
};


#endif // __ARXCLOCK_DAEMON_H__
