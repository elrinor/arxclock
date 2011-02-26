#include "SnoozeDialog.h"
#include "ui_SnoozeDialog.h"

SnoozeDialog::SnoozeDialog(QWidget *parent): 
  QDialog(parent),
  mUi(new Ui::SnoozeDialog())
{
  mUi->setupUi(this);
}

SnoozeDialog::~SnoozeDialog() {
  return;
}

QDateTime SnoozeDialog::snoozeTime() const {
  return mUi->timeEdit->dateTime();
}
