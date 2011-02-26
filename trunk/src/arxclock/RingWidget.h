#ifndef ARXCLOCK_RING_WIDGET_H
#define ARXCLOCK_RING_WIDGET_H

#include "config.h"
#include <boost/shared_ptr.hpp>
#include <QWidget>
#include <QScopedPointer>
#include "Alarm.h"
#include "MainWidget.h"

class QMenu;

namespace Ui {
  class RingWidget;
}

namespace arxclock {
  class MainWidget;

// -------------------------------------------------------------------------- //
// RingWidgetKeys
// -------------------------------------------------------------------------- //
  class RingWidgetKeys {
  public:
    static QString keySize() { return "ring_widget/size"; }
    static QString keyPos () { return "ring_widget/position"; }
  };


// -------------------------------------------------------------------------- //
// RingWidget
// -------------------------------------------------------------------------- //
  class RingWidget: public QWidget, private RingWidgetKeys {
    Q_OBJECT;

  public:
    RingWidget(QSettings *settings, boost::shared_ptr<Alarm> alarm, MainWidget *mainWidget, QWidget *parent = NULL);

  protected:
    virtual void closeEvent(QCloseEvent *event);

  private slots:
    void on_snooze5Button_clicked();
    void snooze10();
    void snooze15();
    void snooze30();
    void snooze60();
    void snoozeDialog();
    void on_moreButton_clicked();

  private:
    QScopedPointer<Ui::RingWidget> mUi;

    QSettings *mSettings;
    boost::shared_ptr<Alarm> mAlarm;
    MainWidget *mMainWidget;

    int mSnoozeSecs;
    QMenu *mMoreMenu;
  };

} // namespace arxclock

#endif // ARXCLOCK_RING_WIDGET_H
