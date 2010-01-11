#include "MainDialog.h"
#include "Alarm.h"
#include "AlarmDialog.h"

MainDialog::MainDialog(QSettings* settings): settings(settings) {
  treeView = new QTreeView();
  treeView->setRootIsDecorated(false);
  treeView->setAlternatingRowColors(true);
  treeView->installEventFilter(this);

  this->itemModel = new QStandardItemModel(0, 3, this);
  this->itemModel->setHeaderData(0, Qt::Horizontal, "On");
  this->itemModel->setHeaderData(1, Qt::Horizontal, "Message");
  this->itemModel->setHeaderData(2, Qt::Horizontal, "Type");

  QList<QVariant> alarmIds = settings->value(KEY_ALARM_IDS, QList<QVariant>()).toList();
  for(int i = 0; i < alarmIds.size(); i++)
    itemModel->appendRow(Alarm::loadFromSettings(settings, alarmIds[i]).toRow());
  treeView->setModel(itemModel);
  for(int i = 0; i < itemModel->columnCount(); i++)
    treeView->resizeColumnToContents(i);
  connect(treeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(editClicked()));
  connect(treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(clicked(const QModelIndex&)));
  connect(treeView, SIGNAL() )
    treeView->clicked()

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
  layout->addWidget(treeView);
  layout->addLayout(buttonsLayout);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
  setWindowTitle(QString("ArXClock v") + ARXCLOCK_VERSION);

  resize(settings->value(KEY_SETTINGDLG_SIZE, QSize(640, 480)).toSize());
  move(settings->value(KEY_SETTINGDLG_POS, QApplication::desktop()->screenGeometry().center() - 
    QPoint(640, 480) / 2).toPoint());
}

void MainDialog::closeEvent(QCloseEvent *event) {
  settings->setValue(KEY_SETTINGDLG_SIZE, this->size());
  settings->setValue(KEY_SETTINGDLG_POS, this->pos());
  event->accept();
}

void MainDialog::addClicked() {
  AlarmDialog* dlg = new AlarmDialog(this->settings);
  dlg->exec();
  if(dlg->result() == Accepted) {
    Alarm alarm = dlg->getAlarm();

    QString s1 = alarm.mNextRunTime.toString();

    alarm.saveToSettings(this->settings);

    QList<QVariant> alarmIds = settings->value(KEY_ALARM_IDS).toList();
    alarmIds.append(alarm.getId());
    this->settings->setValue(KEY_ALARM_IDS, alarmIds);

    this->itemModel->appendRow(alarm.toRow());
  }
  delete dlg;
}

void MainDialog::editClicked() {
  QModelIndex idx = this->treeView->currentIndex();
  if(idx.column() == -1 || idx.row() == -1)
    return;

  AlarmDialog* dlg = new AlarmDialog(this->settings, 
    Alarm::loadFromSettings(this->settings, idx.data(Qt::UserRole)));
  dlg->exec();
  if(dlg->result() == Accepted) {
    Alarm alarm = dlg->getAlarm();
    alarm.saveToSettings(this->settings);
    int r = idx.row();
    this->itemModel->removeRow(r);
    this->itemModel->insertRow(r, alarm.toRow());
    //this->treeView->setSelection(this->treeView->visualRect(idx), )
    this->treeView->setCurrentIndex(idx);
  }
  delete dlg;
}

void MainDialog::deleteClicked() {
  QModelIndex idx = this->treeView->currentIndex();
  if(idx.column() == -1 || idx.row() == -1)
    return;

  QList<QVariant> alarmIds = settings->value(KEY_ALARM_IDS).toList();
  for(int i = 0; i < alarmIds.size(); i++) {
    if(alarmIds[i].toString() == idx.data(Qt::UserRole)) {
      alarmIds.removeAt(i);
      break;
    }
  }
  this->settings->setValue(KEY_ALARM_IDS, alarmIds);

  this->itemModel->removeRow(idx.row());
}

bool MainDialog::eventFilter(QObject* sender, QEvent* e) {
  bool filtered = false;
  if(e->type() == QEvent::KeyPress) {
    QKeyEvent* k = static_cast<QKeyEvent*>(e);
    if(k->key() == Qt::Key_Delete) {
      deleteClicked();
      filtered = true;
    }
  }
  return filtered;
}

void MainDialog::clicked(const QModelIndex& idx) {
  if(idx.column() == -1 || idx.row() == -1)
    return;

  if(idx.column() == 0) {
    Alarm alarm = Alarm::loadFromSettings(this->settings, idx.data(Qt::UserRole));
    alarm.mEnabled = !alarm.mEnabled;
    alarm.saveToSettings(this->settings);
    int r = idx.row();
    int c = idx.column();
    this->itemModel->removeRow(r);
    this->itemModel->insertRow(r, alarm.toRow());
    this->treeView->setCurrentIndex(this->itemModel->index(r, c));
  }
}

