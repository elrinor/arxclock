#ifndef __ARXCLOCK_ALARM_PARAMS_DIALOG_H__
#define __ARXCLOCK_ALARM_PARAMS_DIALOG_H__

#include "config.h"
#include <boost/shared_ptr.hpp>
#include <QtGui>
#include "Alarm.h"
#include "Sound.h"

// -------------------------------------------------------------------------- //
// AlarmParamsDialogKeys
// -------------------------------------------------------------------------- //
class AlarmParamsDialogKeys {
public:
  static QString keySize   () { return "alarm_params_dialog/size"; }
  static QString keyPos    () { return "alarm_params_dialog/position"; }
  static QString keyLastDir() { return "alarm_params_dialog/last_dir"; }
};


// -------------------------------------------------------------------------- //
// AlarmParamsDialog
// -------------------------------------------------------------------------- //
class AlarmParamsDialog: public QDialog, private AlarmParamsDialogKeys {
  Q_OBJECT;

public:
  AlarmParamsDialog(QSettings* settings, boost::shared_ptr<Alarm> alarm, QWidget* parent = NULL);

protected:
  virtual void closeEvent(QCloseEvent *event);

private slots:
  void typeIndexChanged(int index);
  void doAccept();
  void openAudioFile();
  void removeAudioFile();
  void playStopClick();

private:
  QAbstractButton* createWeeklyButton(const QString& text);

  QSettings* mSettings;

  boost::shared_ptr<Alarm> mAlarm;
  
  QComboBox* mTypeBox;
  QLineEdit* mNameEdit;

  QLabel* mPreLabel;
  QLabel* mPostLabel;
  QDateTimeEdit* mDateTimeEdit;
  
  QTextEdit* mMessageEdit;
  QLabel* mAudioFile;

  QComboBox* mCommandLineBox;

  QToolButton* mPlayStopButton;

  Sound* mSound;

  QWidget* mWeeklyWidget;
  QAbstractButton* mWeeklyButtons[7];
};


#endif // __ARXCLOCK_ALARM_PARAMS_DIALOG_H__
