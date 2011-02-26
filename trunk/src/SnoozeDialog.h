#ifndef ARXCLOCK_SNOOZE_DIALOG_H
#define ARXCLOCK_SNOOZE_DIALOG_H

#include "config.h"
#include <QDialog>
#include <QScopedPointer>
#include <QDateTime>

namespace Ui {
  class SnoozeDialog;
}

class SnoozeDialog: public QDialog {
  Q_OBJECT;
public:
  SnoozeDialog(QWidget *parent = NULL);

  ~SnoozeDialog();

  QDateTime snoozeTime() const;

private:
  QScopedPointer<Ui::SnoozeDialog> mUi;
};

#endif // ARXCLOCK_SNOOZE_DIALOG_H
