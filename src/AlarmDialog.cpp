#include "AlarmDialog.h"

QAbstractButton* AlarmDialog::createWeeklyButton(const QString& text) {
  QToolButton* result = new QToolButton();
  result->setToolButtonStyle(Qt::ToolButtonTextOnly);
  result->setCheckable(true);
  result->setMinimumWidth(30);
  result->setText(text);
  return result;
}

AlarmDialog::AlarmDialog(QSettings* settings, const Alarm& alarm): settings(settings) {
  typeBox = new QComboBox();
  for(int i = 0; i <= Alarm::ALARM_LAST_TYPE; i++)
    typeBox->addItem(Alarm::alarmTypeToString(static_cast<Alarm::Type>(i)), i);
  connect(typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeIndexChanged(int)));

  QLabel* typeLabel = new QLabel("&Type:");
  typeLabel->setBuddy(typeBox);

  QLabel* nameLabel = new QLabel("&Name:");

  QHBoxLayout* typeLayout = new QHBoxLayout();
  typeLayout->addWidget(typeLabel);
  typeLayout->addWidget(typeBox);
  typeLayout->addStretch(1);

  QGroupBox* typeBox = new QGroupBox(QString("&General"));
  typeBox->setLayout(typeLayout);

  this->preLabel = new QLabel();
  this->preLabel->setMinimumWidth(70);
  this->preLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  this->dateTimeEdit = new QDateTimeEdit();
  this->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

  this->postLabel = new QLabel();
  
  QHBoxLayout* dateTimeLayout = new QHBoxLayout();
  dateTimeLayout->addWidget(preLabel);
  dateTimeLayout->addWidget(dateTimeEdit);
  dateTimeLayout->addWidget(postLabel, 1);

  QHBoxLayout* weeklyLayout = new QHBoxLayout();
  weeklyLayout->addWidget(new QLabel("on the following days "));
  for(int i = 0; i < 7; i++) {
    this->weeklyButtons[i] = createWeeklyButton(Alarm::dayName(i));
    weeklyLayout->addWidget(this->weeklyButtons[i]);
  }
  weeklyLayout->addStretch(1);
  weeklyLayout->setContentsMargins(0, 0, 0, 0);
  this->weeklyWidget = new QWidget();
  this->weeklyWidget->setLayout(weeklyLayout);

  this->textMessage = new QTextEdit(alarm.mMessage);
  this->textMessage->setMaximumHeight(70);
  QLabel* messageLabel = new QLabel("Show mMessage");
  messageLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

  this->audioFile = new QLabel(alarm.mFileName);
  QToolButton* openButton = new QToolButton();
  openButton->setIcon(QIcon(":/icon_open.png"));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openAudioFile()));

  this->playStopButton = new QToolButton();
  this->playStopButton->setIcon(QIcon(":/icon_play.png"));
  connect(this->playStopButton, SIGNAL(clicked()), this, SLOT(playStopClick()));

  QToolButton* removeButton = new QToolButton();
  removeButton->setIcon(QIcon(":/icon_remove.png"));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeAudioFile()));

  QHBoxLayout* audioLayout = new QHBoxLayout();
  audioLayout->addWidget(this->audioFile, 1);
  audioLayout->addWidget(openButton);
  audioLayout->addWidget(this->playStopButton);
  audioLayout->addWidget(removeButton);

  QGridLayout* gridLayout = new QGridLayout();
  gridLayout->addWidget(messageLabel,             0, 0);
  gridLayout->addWidget(this->textMessage,        0, 1);
  gridLayout->addWidget(new QLabel("Play Sound"), 1, 0);
  gridLayout->addLayout(audioLayout,              1, 1);

  QVBoxLayout* settingsLayout = new QVBoxLayout();
  settingsLayout->addLayout(dateTimeLayout);
  settingsLayout->addWidget(this->weeklyWidget);
  settingsLayout->addStretch(1);
  settingsLayout->addLayout(gridLayout);

  QGroupBox* settingsBox = new QGroupBox(QString("&Settings"));
  settingsBox->setLayout(settingsLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(typeBox);
  layout->addWidget(settingsBox);
  layout->addWidget(buttonBox);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
  setWindowTitle(QString("Alarm..."));

  this->sound = NULL;

  resize(settings->value(KEY_ALARMDLG_SIZE, QSize(640, 480)).toSize());
  move(settings->value(KEY_ALARMDLG_POS, QApplication::desktop()->screenGeometry().center() - 
    QPoint(640, 480) / 2).toPoint());

  this->dateTimeEdit->setDateTime(alarm.mTime);
  for(int i = 0; i < alarm.mWeekMask.size(); i++)
    this->weeklyButtons[i]->setChecked(alarm.mWeekMask[i].toBool());

  for(int i = 0; i < this->typeBox->count(); i++) {
    if(this->typeBox->itemData(i).toInt() == alarm.mType) {
      this->typeBox->setCurrentIndex(i);
      break;
    }
  }

  ralarm.mId = alarm.mId;
}

void AlarmDialog::closeEvent(QCloseEvent *event) {
  settings->setValue(KEY_ALARMDLG_SIZE, this->size());
  settings->setValue(KEY_ALARMDLG_POS, this->pos());
  event->accept();
}

void AlarmDialog::typeIndexChanged(int index) {
  Alarm::Type type = static_cast<Alarm::Type>(this->typeBox->itemData(index).toInt());
  
  if(type == Alarm::ALARM_WEEKLY)
    this->weeklyWidget->show();
  else
    this->weeklyWidget->hide();

  this->dateTimeEdit->setMaximumDate(QDate(7999, 12, 31));
  this->dateTimeEdit->setMinimumDate(QDate(1752,  9, 14));

  if(type == Alarm::ALARM_COUNTDOWN)
    this->dateTimeEdit->setDateTime(QDateTime(QDate::currentDate()));

  switch(type) {
  case Alarm::ALARM_COUNTDOWN:
    this->preLabel->setText("Run after");
    this->dateTimeEdit->setDisplayFormat("hh:mm:ss");
    this->postLabel->setText("");
    break;
  case Alarm::ALARM_ONCE:
    this->preLabel->setText("Run once at");
    this->dateTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    this->postLabel->setText("");
    break;
  case Alarm::ALARM_MINUTELY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("ss");
    this->postLabel->setText("seconds every minute");
    break;
  case Alarm::ALARM_HOURLY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("mm:ss");
    this->postLabel->setText("every hour");
    break;
  case Alarm::ALARM_DAYLY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("hh:mm:ss");
    this->postLabel->setText("every day");
    break;
  case Alarm::ALARM_WEEKLY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("hh:mm:ss");
    this->postLabel->setText("every week");
    break;
  case Alarm::ALARM_MONTHLY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("dd hh:mm:ss");
    this->postLabel->setText("every month");
    break;
  case Alarm::ALARM_ANNUALLY:
    this->preLabel->setText("Run at");
    this->dateTimeEdit->setDisplayFormat("dd.MM hh:mm:ss");
    this->postLabel->setText("every year");
    break;
  }
}

void AlarmDialog::doAccept() {
  ralarm.mType = static_cast<Alarm::Type>(this->typeBox->itemData(this->typeBox->currentIndex()).toInt());
  ralarm.mTime = this->dateTimeEdit->dateTime();
  ralarm.mMessage = this->textMessage->toPlainText();
  ralarm.mFileName = this->audioFile->text();

  ralarm.mWeekMask.clear();
  for(int i = 0; i < 7; i++)
    ralarm.mWeekMask.append(this->weeklyButtons[i]->isChecked());

  if(ralarm.mType == Alarm::ALARM_COUNTDOWN) {
    QDateTime now = QDateTime::currentDateTime();
    now = now.addSecs(ralarm.mTime.time().second() + 60 * ralarm.mTime.time().minute() + 
      60 * 60 * ralarm.mTime.time().hour());
    ralarm.mType = Alarm::ALARM_ONCE;
    ralarm.mTime = now;
  }

  ralarm.recalculateNextRunTime();

  accept();
}

void AlarmDialog::openAudioFile() {
  if(this->sound != NULL) {
    delete this->sound;
    this->sound = NULL;
    this->playStopButton->setIcon(QIcon(":/icon_play.png"));
  }
  QString fileName = QFileDialog::getOpenFileName(this, "Open Audio File...", 
    this->settings->value(KEY_LAST_OPENDIR).toString(), "Audio Files (*.wav *.mp3);;All Files (*.*)"); 
  if(!fileName.isEmpty()) {
    this->audioFile->setText(fileName);
    this->settings->setValue(KEY_LAST_OPENDIR, fileName);
  }
}

void AlarmDialog::removeAudioFile() {
  if(this->sound != NULL) {
    delete this->sound;
    this->sound = NULL;
  }
  
  this->playStopButton->setIcon(QIcon(":/icon_play.png"));
  this->audioFile->setText("");
}

void AlarmDialog::playStopClick() {
  if(this->sound == NULL) {
    if(!QFile::exists(this->audioFile->text()))
      return;

    this->sound = new Sound(this->audioFile->text(), this); 
    this->sound->play();
    this->playStopButton->setIcon(QIcon(":/icon_stop.png"));
  } else {
    delete this->sound;
    this->sound = 0;
    this->playStopButton->setIcon(QIcon(":/icon_play.png"));
  }
}
