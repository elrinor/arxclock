#include "RingWidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <cstdlib> /* for system(). */
#include "Alarm.h"
#include "Sound.h"
#include "SnoozeDialog.h"
#include "MainWidget.h"
#include "ui_RingWidget.h"

RingWidget::RingWidget(QSettings *settings, boost::shared_ptr<Alarm> alarm, MainWidget *mainWidget, QWidget *parent): 
  QWidget(parent),
  mUi(new Ui::RingWidget),
  mSettings(settings), 
  mAlarm(alarm), 
  mMainWidget(mainWidget), 
  mSnoozeSecs(0) 
{
  mUi->setupUi(this);

  mUi->textEdit->setPlainText(alarm->message());

  mMoreMenu = new QMenu(this);
  mMoreMenu->addAction(tr("Snooze 10 minutes..."), this, SLOT(snooze10()));
  mMoreMenu->addAction(tr("Snooze 15 minutes..."), this, SLOT(snooze15()));
  mMoreMenu->addAction(tr("Snooze 30 minutes..."), this, SLOT(snooze30()));
  mMoreMenu->addAction(tr("Snooze 60 minutes..."), this, SLOT(snooze60()));
  mMoreMenu->addAction(tr("More..."), this, SLOT(snoozeDialog()));

  Sound *sound = new Sound(alarm->fileName(), this);
  sound->play();

  if(!alarm->commandLine().isEmpty())
    system(alarm->commandLine().toAscii().constData()); /* TODO: use QProcess. */

  setWindowTitle(alarm->name());

  resize(settings->value(keySize(), size()).toSize());
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

void RingWidget::on_snooze5Button_clicked() {
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

void RingWidget::on_moreButton_clicked() {
  mMoreMenu->popup(mapToGlobal(mUi->moreButton->pos() + QPoint(mUi->moreButton->width(), 0)));
}
