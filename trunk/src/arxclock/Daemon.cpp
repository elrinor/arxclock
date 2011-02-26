#include "Daemon.h"
#include <cassert>
#include <QApplication>
#include "MainWidget.h"
#include "Alarm.h"

namespace arxclock {

  Daemon::Daemon(QSettings *settings): mSettings(settings), mAlarmManager(settings), mMainWidget(NULL) {
    assert(settings != NULL);

    mTrayIcon = new QSystemTrayIcon(qApp->windowIcon(), this);
    mTrayIcon->setToolTip("arxclock");
    mTrayIcon->show();

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    mTimer->start(1000);

    mMainWidget = new MainWidget(mSettings, &mAlarmManager);

    connect(mTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
      this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
  }

  Daemon::~Daemon() {
    delete mMainWidget;
  }

  void Daemon::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch(reason) {
    case QSystemTrayIcon::Context:
      break;
    case QSystemTrayIcon::DoubleClick:
      mMainWidget->show();
      mMainWidget->raise();
      mMainWidget->showNormal();
      mMainWidget->activateWindow();
      break;
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Unknown:
    default:
      break;
    }
  }

  void Daemon::timeout() {
    bool activated = false;

    QDateTime now = QDateTime::currentDateTime();
    foreach(boost::shared_ptr<Alarm> alarm, mAlarmManager.alarms()) {
      if(alarm->active())
        continue;
    
      if(now.secsTo(alarm->nextRunTime()) >= 0)
        continue;

      if(alarm->enabled()) {
        activated = true;
        alarm->setActive(true);
        RingWidget *ringWidget = new RingWidget(mSettings, alarm, mMainWidget);
        ringWidget->setAttribute(Qt::WA_DeleteOnClose);
        ringWidget->show();
      }
    }

    if(activated)
      mMainWidget->updateNextRunTimes();
  }

} // namespace arxclock
