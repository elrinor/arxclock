#include "MainWidget.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QStandardItemModel>
#include "Alarm.h"
#include "AlarmManager.h"
#include "AlarmParamsDialog.h"
#include "ui_MainWidget.h"

namespace arxclock {

  namespace {

    QString alarmNextRun(boost::shared_ptr<Alarm> alarm) {
      if(alarm->active()) {
        return MainWidget::tr("<running>");
      } else if(alarm->nextRunTime() == Alarm::never()) {
        return MainWidget::tr("<never>");
      } else {
        return MainWidget::tr("%1 at %2").arg(alarm->nextRunTime().toString(MainWidget::tr("dd.MM.yy"))).arg(alarm->nextRunTime().toString(MainWidget::tr("hh:mm:ss")));
      }
    }

    QList<QStandardItem *> alarmToRow(boost::shared_ptr<Alarm> alarm) {
      QList<QStandardItem *> result; 

      QStandardItem *enabledItem = new QStandardItem();
      enabledItem->setData(alarm->id(), Qt::UserRole);
      enabledItem->setData(alarm->enabled() ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
      enabledItem->setEditable(false);

      QStandardItem *nameItem = new QStandardItem(alarm->name());
      nameItem->setData(alarm->id(), Qt::UserRole);
      nameItem->setEditable(false);

      QStandardItem *typeItem = new QStandardItem(alarm->toString());
      typeItem->setData(alarm->id(), Qt::UserRole);
      typeItem->setEditable(false);

      QStandardItem *nextRunItem = new QStandardItem(alarmNextRun(alarm));
      nextRunItem->setData(alarm->id(), Qt::UserRole);
      nextRunItem->setEditable(false);

      return result << enabledItem << nameItem << typeItem << nextRunItem;
    }

  } // namespace `anonymous-namespace`

  MainWidget::MainWidget(QSettings *settings, AlarmManager *alarmManager, QWidget *parent): 
    QWidget(parent),
    mUi(new Ui::MainWidget()),
    mSettings(settings), 
    mAlarmManager(alarmManager)
  {
    mUi->setupUi(this);

    mUi->treeView->installEventFilter(this);

    mItemModel = new QStandardItemModel(0, 4, this);
    mItemModel->setHeaderData(0, Qt::Horizontal, "On");
    mItemModel->setHeaderData(1, Qt::Horizontal, "Name");
    mItemModel->setHeaderData(2, Qt::Horizontal, "Type");
    mItemModel->setHeaderData(3, Qt::Horizontal, "Next Run");

    foreach(boost::shared_ptr<Alarm> alarm, mAlarmManager->alarms())
      mItemModel->appendRow(alarmToRow(alarm));
  
    mUi->treeView->setModel(mItemModel);
    for(int i = 0; i < mItemModel->columnCount(); i++)
      mUi->treeView->resizeColumnToContents(i);
    connect(mUi->treeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_editButton_clicked()));

    setWindowTitle(windowTitle().arg(ARXCLOCK_VERSION));

    resize(settings->value(keySize(), size()).toSize());
    move(settings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(640, 480) / 2).toPoint());
  }

  MainWidget::~MainWidget() {
    return;
  }

  void MainWidget::updateNextRunTimes() {
    for(int i = 0; i < mItemModel->rowCount(); i++) {
      QModelIndex idx = mItemModel->index(i, 3);
      boost::shared_ptr<Alarm> alarm = mAlarmManager->alarmById(idx.data(Qt::UserRole).toString());
      mItemModel->setData(idx, alarmNextRun(alarm));
    }
  }

  void MainWidget::closeEvent(QCloseEvent *event) {
    mSettings->setValue(keySize(), size());
    mSettings->setValue(keyPos(), pos());
    mAlarmManager->commit();
    event->accept();
  }

  void MainWidget::on_addButton_clicked() {
    boost::array<bool, 7> weekMask;
    std::fill(weekMask.begin(), weekMask.end(), false);

    boost::shared_ptr<Alarm> alarm(new Alarm(
      QDateTime::currentDateTime().toString("yy_MM_dd_hh_mm_ss_zzz") + "_" + QString().setNum(qrand()),
      Alarm::ALARM_DAYLY,
      true,
      "",
      "",
      QDateTime::currentDateTime(),
      weekMask,
      "",
      ""
    ));

    AlarmParamsDialog dlg(mSettings, alarm, this);
    dlg.exec();
    if(dlg.result() == QDialog::Accepted) {
      mAlarmManager->add(alarm);
      mItemModel->appendRow(alarmToRow(alarm));
    }
  }

  void MainWidget::on_editButton_clicked() {
    QModelIndex idx = mUi->treeView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1)
      return;

    boost::shared_ptr<Alarm> alarm = mAlarmManager->alarmById(idx.data(Qt::UserRole).toString());
    AlarmParamsDialog dlg(mSettings, alarm, this);
    dlg.exec();
    if(dlg.result() == QDialog::Accepted) {
      int r = idx.row();
      mItemModel->removeRow(r);
      mItemModel->insertRow(r, alarmToRow(alarm));
      mUi->treeView->setCurrentIndex(idx);
    }
  }

  void MainWidget::on_deleteButton_clicked() {
    QModelIndex idx = mUi->treeView->currentIndex();
    if(idx.column() == -1 || idx.row() == -1)
      return;

    mAlarmManager->remove(mAlarmManager->alarmById(idx.data(Qt::UserRole).toString()));
    mItemModel->removeRow(idx.row());
  }

  bool MainWidget::eventFilter(QObject * /* sender */, QEvent *e) {
    bool filtered = false;
    if(e->type() == QEvent::KeyPress) {
      QKeyEvent *k = static_cast<QKeyEvent *>(e);
      switch(k->key()) {
      case Qt::Key_Delete:
        on_deleteButton_clicked();
        filtered = true;
        break;
      case Qt::Key_Plus:
        on_addButton_clicked();
        filtered = true;
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        on_editButton_clicked();
        filtered = true;
        break;
      case Qt::Key_Space: {
        QModelIndex idx = mUi->treeView->currentIndex();
        if(idx.row() == -1)
          break;
        toggleEnabledSelected();
        break;
      }
      default:
        break;
      }
    }
    return filtered;
  }

  void MainWidget::on_treeView_clicked(const QModelIndex &idx) {
    if(idx.column() == -1 || idx.row() == -1)
      return;

    if(idx.column() == 0)
      toggleEnabledSelected();
  }

  void MainWidget::toggleEnabledSelected() {
    QModelIndex idx = mUi->treeView->currentIndex();
    boost::shared_ptr<Alarm> alarm = mAlarmManager->alarmById(idx.data(Qt::UserRole).toString());
    alarm->setEnabled(!alarm->enabled());

    int r = idx.row();
    int c = idx.column();
    mItemModel->removeRow(r);
    mItemModel->insertRow(r, alarmToRow(alarm));
    mUi->treeView->setCurrentIndex(mItemModel->index(r, c));
  }

} // namespace arxclock
