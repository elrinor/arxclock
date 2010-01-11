#ifndef __ARXCLOCK_RING_DIALOG_H__
#define __ARXCLOCK_RING_DIALOG_H__

#include "config.h"
#include <QtGui>

class RingDialog: public QDialog {
  Q_OBJECT;

public:
  RingDialog(QSettings* settings, QVariant id);

  int getSnoozeSecs() const { return this->snoozeSecs; }

protected:
  virtual void closeEvent(QCloseEvent *event);

private slots:
  void snooze5();
  void snooze10();
  void snooze15();
  void snooze30();
  void snooze60();
  void more();

private:
  int snoozeSecs;
  QMenu* moreMenu;
  QPushButton* moreButton;
  QSettings* settings;
  QVariant id;
};

#endif // __ARXCLOCK_RING_DIALOG_H__
