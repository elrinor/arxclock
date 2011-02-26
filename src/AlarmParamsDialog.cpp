#include "AlarmParamsDialog.h"
#include <cassert>
#include <QDesktopWidget>
#include <QFileDialog>
#include "ui_AlarmParamsDialog.h"
#include "Utility.h"
#include "Sound.h"

AlarmParamsDialog::AlarmParamsDialog(QSettings *settings, boost::shared_ptr<Alarm> alarm, QWidget *parent): 
  QDialog(parent), 
  mUi(new Ui::AlarmParamsDialog()),
  mSettings(settings), 
  mAlarm(alarm)
{
  assert(settings != NULL);
  assert(alarm.get() != NULL);

  mUi->setupUi(this);

  for(int i = Alarm::ALARM_FIRST_TYPE; i <= Alarm::ALARM_LAST_TYPE; i++)
    mUi->typeBox->addItem(Alarm::typeToString(static_cast<Alarm::Type>(i)), i);

  for(int i = 0; i < mUi->typeBox->count(); i++) {
    if(mUi->typeBox->itemData(i).toInt() == alarm->type()) {
      mUi->typeBox->setCurrentIndex(i);
      break;
    }
  }

  mUi->nameEdit->setText(alarm->name());

  mWeeklyButtons[0] = mUi->monButton;
  mWeeklyButtons[1] = mUi->tueButton;
  mWeeklyButtons[2] = mUi->wedButton;
  mWeeklyButtons[3] = mUi->thuButton;
  mWeeklyButtons[4] = mUi->friButton;
  mWeeklyButtons[5] = mUi->satButton;
  mWeeklyButtons[6] = mUi->sunButton;

  mUi->messageEdit->setPlainText(alarm->message());

  mUi->fileEdit->setText(alarm->fileName());

#ifdef Q_WS_WIN
  mUi->commandBox->addItem("rundll32.exe user32.dll, LockWorkStation");
#endif
  mUi->commandBox->setEditText(alarm->commandLine());

  mUi->dateTimeEdit->setDateTime(alarm->time());
  for(unsigned i = 0; i < alarm->weekMask().size(); i++)
    mWeeklyButtons[i]->setChecked(alarm->weekMask(i));

  resize(mSettings->value(keySize(), size()).toSize());
  move(mSettings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(width(), height()) / 2).toPoint());
}

AlarmParamsDialog::~AlarmParamsDialog() {
  return;
}

void AlarmParamsDialog::closeEvent(QCloseEvent *event) {
  savePosition();
  
  QDialog::closeEvent(event);
}

void AlarmParamsDialog::savePosition() {
  mSettings->setValue(keySize(), size());
  mSettings->setValue(keyPos(), pos());
}

void AlarmParamsDialog::on_typeBox_currentIndexChanged(int index) {
  Alarm::Type type = static_cast<Alarm::Type>(mUi->typeBox->itemData(index).toInt());
  
  if(type == Alarm::ALARM_WEEKLY)
    mUi->weeklyWidget->show();
  else
    mUi->weeklyWidget->hide();

  mUi->dateTimeEdit->setMaximumDate(QDate(7999, 12, 31));
  mUi->dateTimeEdit->setMinimumDate(QDate(1752,  9, 14));

  if(type == Alarm::ALARM_COUNTDOWN)
    mUi->dateTimeEdit->setDateTime(QDateTime(QDate::currentDate()));

  switch(type) {
  case Alarm::ALARM_COUNTDOWN:
    mUi->prefixLabel->setText(tr("Run after"));
    mUi->dateTimeEdit->setDisplayFormat(tr("hh:mm:ss", "ALARM_COUNTDOWN"));
    mUi->suffixLabel->setText(tr("", "ALARM_COUNTDOWN"));
    break;
  case Alarm::ALARM_ONCE:
    mUi->prefixLabel->setText(tr("Run once at"));
    mUi->dateTimeEdit->setDisplayFormat(tr("dd.MM.yyyy hh:mm:ss"));
    mUi->suffixLabel->setText(tr("", "ALARM_ONCE"));
    break;
  case Alarm::ALARM_MINUTELY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_MINUTELY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("ss"));
    mUi->suffixLabel->setText(tr("seconds every minute"));
    break;
  case Alarm::ALARM_HOURLY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_HOURLY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("mm:ss"));
    mUi->suffixLabel->setText(tr("every hour"));
    break;
  case Alarm::ALARM_DAYLY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_DAYLY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("hh:mm:ss", "ALARM_DAYLY"));
    mUi->suffixLabel->setText(tr("every day"));
    break;
  case Alarm::ALARM_WEEKLY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_WEEKLY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("hh:mm:ss", "ALARM_WEEKLY"));
    mUi->suffixLabel->setText(tr("every week"));
    break;
  case Alarm::ALARM_MONTHLY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_MONTHLY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("dd hh:mm:ss"));
    mUi->suffixLabel->setText(tr("every month"));
    break;
  case Alarm::ALARM_ANNUALLY:
    mUi->prefixLabel->setText(tr("Run at", "ALARM_ANNUALLY"));
    mUi->dateTimeEdit->setDisplayFormat(tr("dd.MM hh:mm:ss"));
    mUi->suffixLabel->setText(tr("every year"));
    break;
  }
}

void AlarmParamsDialog::accept() {
  Alarm::Type type = static_cast<Alarm::Type>(mUi->typeBox->itemData(mUi->typeBox->currentIndex()).toInt());
  QDateTime time = mUi->dateTimeEdit->dateTime();
  if(type == Alarm::ALARM_COUNTDOWN) {
    type = Alarm::ALARM_ONCE;
    time = QDateTime::currentDateTime().addSecs(time.time().second() + 60 * time.time().minute() + 60 * 60 * time.time().hour());
  }

  mAlarm->setType(type);
  mAlarm->setTime(time);
  mAlarm->setMessage(mUi->messageEdit->toPlainText());
  mAlarm->setFileName(mUi->fileEdit->text());
  mAlarm->setName(mUi->nameEdit->text());
  mAlarm->setCommandLine(mUi->commandBox->currentText());

  for(int i = 0; i < 7; i++)
    mAlarm->setWeekMask(i, mWeeklyButtons[i]->isChecked());

  mAlarm->recalculateNextRunTime();

  savePosition();
  
  QDialog::accept();
}

void AlarmParamsDialog::reject() {
  savePosition();
  
  QDialog::reject();
}

void AlarmParamsDialog::on_openButton_clicked() {
  if(!mSound.isNull()) {
    mSound.reset();
    mUi->playButton->setIcon(QIcon(":/icon_play.png"));
  }
  
  QString fileName = QFileDialog::getOpenFileName(
    this, 
    tr("Open Audio File..."), 
    mSettings->value(keyLastDir()).toString(), 
    tr("Audio Files (*.wav *.mp3 *.flac *.ape *.wv *.ogg);;All Files (*.*)")
  ); 
  if(fileName.isEmpty())
    return;
  
  mUi->fileEdit->setText(fileName);
  mSettings->setValue(keyLastDir(), fileName);
}

void AlarmParamsDialog::on_removeButton_clicked() {
  if(!mSound.isNull())
    mSound.reset();
  
  mUi->playButton->setIcon(QIcon(":/icon_play.png"));
  mUi->fileEdit->setText(QString());
}

void AlarmParamsDialog::on_playButton_clicked() {
  if(mSound.isNull()) {
    if(!QFile::exists(mUi->fileEdit->text()))
      return;

    mSound.reset(new Sound(mUi->fileEdit->text(), this)); 
    mSound->play();
    mUi->playButton->setIcon(QIcon(":/icon_stop.png"));
  } else {
    mSound.reset();
    mUi->playButton->setIcon(QIcon(":/icon_play.png"));
  }
}
