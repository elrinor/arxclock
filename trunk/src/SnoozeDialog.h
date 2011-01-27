#ifndef ARXCLOCK_SNOOZE_DIALOG_H
#define ARXCLOCK_SNOOZE_DIALOG_H

#include "config.h"
#include <QtGui>

class SnoozeDialog: public QDialog {
  Q_OBJECT;

public:
  SnoozeDialog(QWidget* parent = NULL): QDialog(parent) {
    mDateTimeEdit = new QDateTimeEdit();
    mDateTimeEdit->setDisplayFormat("hh:mm:ss");

    QLabel* snoozeLabel = new QLabel("Snooze time: ");
    snoozeLabel->setBuddy(mDateTimeEdit);

    QHBoxLayout* snoozeLayout = new QHBoxLayout();
    snoozeLayout->addWidget(snoozeLabel);
    snoozeLayout->addWidget(mDateTimeEdit);

    QGroupBox* snoozeBox = new QGroupBox(QString("&Snooze"));
    snoozeBox->setLayout(snoozeLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(snoozeBox);
    layout->addWidget(buttonBox);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
    setWindowTitle(QString("Snooze..."));
  }

  QDateTime snoozeTime() const {
    return mDateTimeEdit->dateTime();
  }

private:
  QDateTimeEdit* mDateTimeEdit;
};


#endif // ARXCLOCK_SNOOZE_DIALOG_H
