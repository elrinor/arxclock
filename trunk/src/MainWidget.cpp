#include "MainWidget.h"
#include "Alarm.h"
#include "AlarmParamsDialog.h"

MainWidget::MainWidget(QSettings* settings, AlarmManager* alarmManager): mSettings(settings), mAlarmManager(alarmManager) {
  mTreeView = new QTreeView();
  mTreeView->setRootIsDecorated(false);
  mTreeView->setAlternatingRowColors(true);
  mTreeView->installEventFilter(this);

  mItemModel = new QStandardItemModel(0, 4, this);
  mItemModel->setHeaderData(0, Qt::Horizontal, "On");
  mItemModel->setHeaderData(1, Qt::Horizontal, "Name");
  mItemModel->setHeaderData(2, Qt::Horizontal, "Type");
  mItemModel->setHeaderData(3, Qt::Horizontal, "Next Run");

  foreach(boost::shared_ptr<Alarm> alarm, mAlarmManager->alarms())
    mItemModel->appendRow(alarmToRow(alarm));
  
  mTreeView->setModel(mItemModel);
  for(int i = 0; i < mItemModel->columnCount(); i++)
    mTreeView->resizeColumnToContents(i);
  connect(mTreeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(editClicked()));
  connect(mTreeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(clicked(const QModelIndex&)));

  QPushButton *addButton = new QPushButton("&Add...");
  connect(addButton, SIGNAL(clicked()), this, SLOT(addClicked()));

  QPushButton *editButton = new QPushButton("&Edit");
  connect(editButton, SIGNAL(clicked()), this, SLOT(editClicked()));

  QPushButton *deleteButton = new QPushButton("&Delete");
  connect(deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));

  QPushButton *closeButton = new QPushButton("&Close");
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(addButton);
  buttonsLayout->addWidget(editButton);
  buttonsLayout->addWidget(deleteButton);
  buttonsLayout->addWidget(closeButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(mTreeView);
  layout->addLayout(buttonsLayout);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
  setWindowTitle(QString("ArXClock v") + ARXCLOCK_VERSION);

  resize(settings->value(keySize(), QSize(640, 480)).toSize());
  move(settings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(640, 480) / 2).toPoint());
}

QList<QStandardItem*> MainWidget::alarmToRow(boost::shared_ptr<Alarm> alarm) {
  QList<QStandardItem*> result; 

  QStandardItem* enabledItem = new QStandardItem();
  enabledItem->setData(alarm->id(), Qt::UserRole);
  enabledItem->setData(alarm->enabled() ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);
  enabledItem->setEditable(false);

  QStandardItem* nameItem = new QStandardItem(alarm->name());
  nameItem->setData(alarm->id(), Qt::UserRole);
  nameItem->setEditable(false);

  QStandardItem* typeItem = new QStandardItem(alarm->toString());
  typeItem->setData(alarm->id(), Qt::UserRole);
  typeItem->setEditable(false);

  QStandardItem* nextRunItem = new QStandardItem(alarmNextRun(alarm));
  nextRunItem->setData(alarm->id(), Qt::UserRole);
  nextRunItem->setEditable(false);

  return result << enabledItem << nameItem << typeItem << nextRunItem;
}

QString MainWidget::alarmNextRun(boost::shared_ptr<Alarm> alarm) {
  return alarm->active() ? "<running>" : alarm->nextRunTime().toString("dd.MM.yy") + " at " + alarm->nextRunTime().toString("hh:mm:ss");
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

void MainWidget::addClicked() {
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

  AlarmParamsDialog* dlg = new AlarmParamsDialog(mSettings, alarm, this);
  dlg->exec();
  if(dlg->result() == QDialog::Accepted) {
    mAlarmManager->add(alarm);
    mItemModel->appendRow(alarmToRow(alarm));
  }
  delete dlg;
}

void MainWidget::editClicked() {
  QModelIndex idx = mTreeView->currentIndex();
  if(idx.column() == -1 || idx.row() == -1)
    return;

  boost::shared_ptr<Alarm> alarm = mAlarmManager->alarmById(idx.data(Qt::UserRole).toString());
  AlarmParamsDialog* dlg = new AlarmParamsDialog(mSettings, alarm, this);
  dlg->exec();
  if(dlg->result() == QDialog::Accepted) {
    int r = idx.row();
    mItemModel->removeRow(r);
    mItemModel->insertRow(r, alarmToRow(alarm));
    mTreeView->setCurrentIndex(idx);
  }
  delete dlg;
}

void MainWidget::deleteClicked() {
  QModelIndex idx = mTreeView->currentIndex();
  if(idx.column() == -1 || idx.row() == -1)
    return;

  mAlarmManager->remove(mAlarmManager->alarmById(idx.data(Qt::UserRole).toString()));
  mItemModel->removeRow(idx.row());
}

bool MainWidget::eventFilter(QObject* /* sender */, QEvent* e) {
  bool filtered = false;
  if(e->type() == QEvent::KeyPress) {
    QKeyEvent* k = static_cast<QKeyEvent*>(e);
    switch(k->key()) {
    case Qt::Key_Delete:
      deleteClicked();
      filtered = true;
      break;
    case Qt::Key_Plus:
      addClicked();
      filtered = true;
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      editClicked();
      filtered = true;
      break;
    case Qt::Key_Space: {
      QModelIndex idx = mTreeView->currentIndex();
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

void MainWidget::clicked(const QModelIndex& idx) {
  if(idx.column() == -1 || idx.row() == -1)
    return;

  if(idx.column() == 0)
    toggleEnabledSelected();
}

void MainWidget::toggleEnabledSelected() {
  QModelIndex idx = mTreeView->currentIndex();
  boost::shared_ptr<Alarm> alarm = mAlarmManager->alarmById(idx.data(Qt::UserRole).toString());
  alarm->setEnabled(!alarm->enabled());

  int r = idx.row();
  int c = idx.column();
  mItemModel->removeRow(r);
  mItemModel->insertRow(r, alarmToRow(alarm));
  mTreeView->setCurrentIndex(mItemModel->index(r, c));
}

