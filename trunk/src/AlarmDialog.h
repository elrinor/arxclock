#ifndef __ARXCLOCK_ALARM_DIALOG_H__
#define __ARXCLOCK_ALARM_DIALOG_H__

#include "config.h"
#include <QtGui>
#include "Alarm.h"
#include "Sound.h"

// rundll32.exe user32.dll, LockWorkStation 

class AlarmDialog: public QDialog {
  Q_OBJECT;

public:
  AlarmDialog(QSettings* settings, const Alarm& alarm = Alarm());

  Alarm getAlarm() const { return this->ralarm; }

protected:
  virtual void closeEvent(QCloseEvent *event);

private slots:
  void typeIndexChanged(int index);
  void doAccept();
  void openAudioFile();
  void removeAudioFile();
  void playStopClick();

private:
  Alarm ralarm;
  QSettings* settings;
  QComboBox *typeBox;

  QLabel* preLabel;
  QLabel* postLabel;
  QDateTimeEdit* dateTimeEdit;
  
  QTextEdit* textMessage;
  QLabel* audioFile;

  QToolButton* playStopButton;

  Sound* sound;

  QAbstractButton* createWeeklyButton(const QString& text);

  QWidget* weeklyWidget;
  QAbstractButton* weeklyButtons[7];
};


#endif // __ARXCLOCK_ALARM_DIALOG_H__
