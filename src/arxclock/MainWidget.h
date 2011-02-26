#ifndef ARXCLOCK_MAIN_WIDGET_H
#define ARXCLOCK_MAIN_WIDGET_H

#include "config.h"
#include <QWidget>
#include <QScopedPointer>

class QSettings;
class QStandardItemModel;
class QModelIndex;

namespace Ui {
  class MainWidget;
}

namespace arxclock {

  class AlarmManager;

// -------------------------------------------------------------------------- //
// MainWidgetKeys
// -------------------------------------------------------------------------- //
  class MainWidgetKeys {
  public:
    static QString keySize() { return "main_widget/size"; }
    static QString keyPos () { return "main_widget/position"; }
  };


// -------------------------------------------------------------------------- //
// MainWidget
// -------------------------------------------------------------------------- //
  class MainWidget: public QWidget, private MainWidgetKeys {
    Q_OBJECT;

  public:
    MainWidget(QSettings *settings, AlarmManager *alarmManager, QWidget *parent = NULL);

    ~MainWidget();

    void updateNextRunTimes();

  protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual bool eventFilter(QObject *sender, QEvent *e);

  private slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();
    void on_treeView_clicked(const QModelIndex &);

  private:
    void toggleEnabledSelected();

    QScopedPointer<Ui::MainWidget> mUi;

    QSettings *mSettings;
    QStandardItemModel *mItemModel;
    AlarmManager *mAlarmManager;
  };

} // namespace arxclock

#endif // ARXCLOCK_MAIN_WIDGET_H
