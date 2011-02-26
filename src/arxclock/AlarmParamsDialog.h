#ifndef ARXCLOCK_ALARM_PARAMS_DIALOG_H
#define ARXCLOCK_ALARM_PARAMS_DIALOG_H

#include "config.h"
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <QScopedPointer>
#include <QDialog>
#include "Alarm.h"

class QCloseEvent;
class QSettings;
class QAbstractButton;

namespace Ui {
    class AlarmParamsDialog;
}

namespace arxclock {
  class Sound;

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
    AlarmParamsDialog(QSettings *settings, boost::shared_ptr<Alarm> alarm, QWidget *parent = NULL);

    ~AlarmParamsDialog();

  protected:
    virtual void closeEvent(QCloseEvent *event);

    void savePosition();

  protected slots:
    void on_typeBox_currentIndexChanged(int index);
    void on_openButton_clicked();
    void on_removeButton_clicked();
    void on_playButton_clicked();
  
    virtual void accept() OVERRIDE;
    virtual void reject() OVERRIDE;

  private:
    QScopedPointer<Ui::AlarmParamsDialog> mUi;
  
    QSettings *mSettings;
  
    boost::shared_ptr<Alarm> mAlarm;

    QScopedPointer<Sound> mSound;

    boost::array<QAbstractButton*, 7> mWeeklyButtons;
  };

} // namespace arxclock

#endif // ARXCLOCK_ALARM_PARAMS_DIALOG_H
