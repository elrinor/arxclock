#ifndef __ARXCLOCK_DAEMON_H__
#define __ARXCLOCK_DAEMON_H__

#include "config.h"
#include <QtGui>
#include "MainDialog.h"
#include "RingDialog.h"

class Daemon: public QObject {
private:
  Q_OBJECT

public:
  Daemon();

private slots:
  void iconActivated(QSystemTrayIcon::ActivationReason reason);
  void timeout();
  void displayMainDialog();

private:
  QTimer* timer;
  QSettings* settings;
  QSystemTrayIcon* trayIcon;
  MainDialog* settingsDialog;

  QSet<QString> ringDlgs;
};


#endif // __ARXCLOCK_DAEMON_H__
