#ifndef ARXCLOCK_DAEMON_H
#define ARXCLOCK_DAEMON_H

#include "config.h"
#include <QObject>
#include <QSystemTrayIcon>
#include "MainWidget.h"
#include "RingWidget.h"
#include "AlarmManager.h"

class QSettings;
class QTimer;

namespace arxclock {
  class MainWidget;

// -------------------------------------------------------------------------- //
// Daemon
// -------------------------------------------------------------------------- //
  class Daemon: public QObject {
    Q_OBJECT
  public:
    Daemon(QSettings *settings);
    ~Daemon();

  private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void timeout();

  private:
    QTimer *mTimer;
    QSettings *mSettings;
    QSystemTrayIcon *mTrayIcon;
    MainWidget *mMainWidget;
    QMenu *mTrayMenu;

    AlarmManager mAlarmManager;
  };

} // namespace arxclock

#endif // ARXCLOCK_DAEMON_H
