#include "RingWidget.h"
#include <cstdlib> /* for system(). */
#include "Alarm.h"
#include "Sound.h"
#include "SnoozeDialog.h"

RingWidget::RingWidget(QSettings* settings, boost::shared_ptr<Alarm> alarm, MainWidget* mainWidget): mSettings(settings), mAlarm(alarm), mMainWidget(mainWidget), mSnoozeSecs(0) {
  QTextEdit* textMessage = new QTextEdit();
  textMessage->setReadOnly(true);
  textMessage->setPlainText(alarm->message());

  QPushButton *snooze5Button = new QPushButton("&Snooze 5 mins...");
  connect(snooze5Button, SIGNAL(clicked()), this, SLOT(snooze5()));

  mMoreButton = new QPushButton("&more...");
  connect(mMoreButton, SIGNAL(clicked()), this, SLOT(more()));

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(snooze5Button);
  buttonsLayout->addWidget(mMoreButton);

  QVBoxLayout* mainLayout = new QVBoxLayout();
  mainLayout->addWidget(textMessage);
  mainLayout->addLayout(buttonsLayout);

  setLayout(mainLayout);

  mMoreMenu = new QMenu(this);
  mMoreMenu->addAction("Snooze 10 minutes...", this, SLOT(snooze10()));
  mMoreMenu->addAction("Snooze 15 minutes...", this, SLOT(snooze15()));
  mMoreMenu->addAction("Snooze 30 minutes...", this, SLOT(snooze30()));
  mMoreMenu->addAction("Snooze 60 minutes...", this, SLOT(snooze60()));
  mMoreMenu->addAction("More...", this, SLOT(snoozeDialog()));

  Sound* sound = new Sound(alarm->fileName(), this);
  sound->play();

  if(!alarm->commandLine().isEmpty())
    system(alarm->commandLine().toAscii().constData());

  setWindowTitle(alarm->name());

  resize(settings->value(keySize(), QSize(320, 100)).toSize());
  move(settings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(320, 100) / 2).toPoint());
}

void RingWidget::closeEvent(QCloseEvent *event) {
  mSettings->setValue(keySize(), size());
  mSettings->setValue(keyPos(), pos());
  
  if(mSnoozeSecs != 0) {
    /* Snoozed. */
    mAlarm->setNextRunTime(QDateTime::currentDateTime().addSecs(mSnoozeSecs));
  } else {
    mAlarm->recalculateNextRunTime();
  }
  mAlarm->setActive(false);

  mMainWidget->updateNextRunTimes();

  event->accept();
}

void RingWidget::snooze5() {
  mSnoozeSecs = 5 * 60;
  close();
}

void RingWidget::snooze10() {
  mSnoozeSecs = 10 * 60;
  close();
}

void RingWidget::snooze15() {
  mSnoozeSecs = 15 * 60;
  close();
}

void RingWidget::snooze30() {
  mSnoozeSecs = 30 * 60;
  close();
}

void RingWidget::snooze60() {
  mSnoozeSecs = 60 * 60;
  close();
}

void RingWidget::snoozeDialog() {
  SnoozeDialog* dlg = new SnoozeDialog(this);
  if(dlg->exec() == QDialog::Accepted) {
    QTime snoozeTime = dlg->snoozeTime().time();
    mSnoozeSecs = snoozeTime.second() + 60 * snoozeTime.minute() + 60 * 60 * snoozeTime.hour();
    close();
  }
  delete dlg;    
}

void RingWidget::more() {
  mMoreMenu->popup(mapToGlobal(mMoreButton->pos() + QPoint(mMoreButton->width(), 0)));
}
