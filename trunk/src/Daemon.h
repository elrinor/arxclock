#ifndef ARXCLOCK_DAEMON_H
#define ARXCLOCK_DAEMON_H

#include "config.h"
#include <QtGui>
#include "MainWidget.h"
#include "RingWidget.h"

class Daemon: public QObject {
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


#endif // ARXCLOCK_DAEMON_H
