#include "Daemon.h"
#include "MainDialog.h"
#include "Alarm.h"

Daemon::Daemon(): settingsDialog(NULL) {
  this->settings = new QSettings("[ArX]Team", "arxclock", this);

  this->trayIcon = new QSystemTrayIcon(dynamic_cast<QApplication*>(QApplication::instance())->windowIcon(), this);
  this->trayIcon->setToolTip("arxclock");
  this->trayIcon->show();

  this->timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
  timer->start(1000);

  connect(this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
    this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void Daemon::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch(reason) {
  case QSystemTrayIcon::Context:
    break;
  case QSystemTrayIcon::DoubleClick:
    displayMainDialog();
    break;
  case QSystemTrayIcon::Trigger:
  case QSystemTrayIcon::MiddleClick:
  case QSystemTrayIcon::Unknown:
  default:
    break;
  }
}

void Daemon::timeout() {
  QDateTime now = QDateTime::currentDateTime();
  QList<QVariant> alarmIds = this->settings->value(KEY_ALARM_IDS, QList<QVariant>()).toList();
  for(int i = 0; i < alarmIds.size(); i++) {
    if(this->ringDlgs.contains(alarmIds[i].toString()))
      continue;

    QDateTime time = QDateTime::fromString(this->settings->value(KEY_NEXTRUN(alarmIds[i])).toString());

    QString s1 = time.toString();
    QString s2 = now.toString();

    if(now.secsTo(time) >= 0)
      continue;

    bool snoozed = false;
    int snoozeTimeSecs = 0;
    if(this->settings->value(KEY_ENABLED(alarmIds[i])).toBool()) {
      RingDialog* ringDlg = new RingDialog(this->settings, alarmIds[i]);
      ringDlg->exec();
      snoozed = ringDlg->result() == QDialog::Accepted;
      snoozeTimeSecs = ringDlg->getSnoozeSecs();
      delete ringDlg;
    }
    if(snoozed) {
      Alarm alarm = Alarm::loadFromSettings(this->settings, alarmIds[i]);
      alarm.mNextRunTime = QDateTime::currentDateTime().addSecs(snoozeTimeSecs);
      this->settings->setValue(KEY_NEXTRUN(alarmIds[i]), alarm.mNextRunTime.toString());
    } else {
      Alarm alarm = Alarm::loadFromSettings(this->settings, alarmIds[i]);
      alarm.recalculateNextRunTime();
      this->settings->setValue(KEY_NEXTRUN(alarmIds[i]), alarm.mNextRunTime.toString());
    }
    this->ringDlgs.remove(alarmIds[i].toString());
    break;
  }
}

void Daemon::displayMainDialog() {
  if(this->settingsDialog == NULL) {
    this->settingsDialog = new MainDialog(this->settings);
    this->settingsDialog->exec();
    delete this->settingsDialog;
    this->settingsDialog = NULL;
  } else
    this->settingsDialog->activateWindow();
}

