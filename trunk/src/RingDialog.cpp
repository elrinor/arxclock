#include "RingDialog.h"
#include "Alarm.h"
#include "Sound.h"

RingDialog::RingDialog(QSettings* settings, QVariant id): settings(settings), id(id) {
  Alarm alarm = Alarm::loadFromSettings(this->settings, this->id);

  QTextEdit* textMessage = new QTextEdit();
  textMessage->setReadOnly(true);
  textMessage->setText(alarm.mMessage);

  QPushButton *snooze5Button = new QPushButton("&Snooze 5 mins...");
  connect(snooze5Button, SIGNAL(clicked()), this, SLOT(snooze5()));

  this->moreButton = new QPushButton("&more...");
  connect(this->moreButton, SIGNAL(clicked()), this, SLOT(more()));

  QHBoxLayout *buttonsLayout = new QHBoxLayout();
  buttonsLayout->addStretch(1);
  buttonsLayout->addWidget(snooze5Button);
  buttonsLayout->addWidget(this->moreButton);

  QVBoxLayout* mainLayout = new QVBoxLayout();
  mainLayout->addWidget(textMessage);
  mainLayout->addLayout(buttonsLayout);

  this->setLayout(mainLayout);

  this->moreMenu = new QMenu(this);
  this->moreMenu->addAction("Snooze 10 minutes...", this, SLOT(snooze10()));
  this->moreMenu->addAction("Snooze 15 minutes...", this, SLOT(snooze15()));
  this->moreMenu->addAction("Snooze 30 minutes...", this, SLOT(snooze30()));
  this->moreMenu->addAction("Snooze 60 minutes...", this, SLOT(snooze60()));

  Sound* sound = new Sound(alarm.mFileName, this);
  sound->play();

  resize(settings->value(KEY_RINGDLG_SIZE, QSize(320, 100)).toSize());
  move(settings->value(KEY_RINGDLG_POS, QApplication::desktop()->screenGeometry().center() - 
    QPoint(320, 100) / 2).toPoint());
}

void RingDialog::closeEvent(QCloseEvent *event) {
  settings->setValue(KEY_RINGDLG_SIZE, this->size());
  settings->setValue(KEY_RINGDLG_POS, this->pos());
  event->accept();
}

void RingDialog::snooze5() {
  this->snoozeSecs = 5 * 60;
  accept();
}

void RingDialog::snooze10() {
  this->snoozeSecs = 10 * 60;
  accept();
}

void RingDialog::snooze15() {
  this->snoozeSecs = 15 * 60;
  accept();
}

void RingDialog::snooze30() {
  this->snoozeSecs = 30 * 60;
  accept();
}

void RingDialog::snooze60() {
  this->snoozeSecs = 60 * 60;
  accept();
}

void RingDialog::more() {
  
  this->moreMenu->popup(mapToGlobal(this->moreButton->pos() + QPoint(this->moreButton->width(), 0)));
}
