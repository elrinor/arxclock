#include "AlarmParamsDialog.h"
#include "Utility.h"

QAbstractButton* AlarmParamsDialog::createWeeklyButton(const QString& text) {
  QToolButton* result = new QToolButton();
  result->setToolButtonStyle(Qt::ToolButtonTextOnly);
  result->setCheckable(true);
  result->setMinimumWidth(30);
  result->setText(text);
  return result;
}

AlarmParamsDialog::AlarmParamsDialog(QSettings* settings, boost::shared_ptr<Alarm> alarm, QWidget* parent): QDialog(parent), mSettings(settings), mAlarm(alarm) {
  mTypeBox = new QComboBox();
  for(int i = Alarm::ALARM_FIRST_TYPE; i <= Alarm::ALARM_LAST_TYPE; i++)
    mTypeBox->addItem(Alarm::typeToString(static_cast<Alarm::Type>(i)), i);
  connect(mTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(typeIndexChanged(int)));

  QLabel* typeLabel = new QLabel("&Type:");
  typeLabel->setBuddy(mTypeBox);

  mNameEdit = new QLineEdit(alarm->name());
 
  QLabel* nameLabel = new QLabel("&Name:");
  nameLabel->setBuddy(mNameEdit);

  QHBoxLayout* typeLayout = new QHBoxLayout();
  typeLayout->addWidget(typeLabel);
  typeLayout->addWidget(mTypeBox);
  typeLayout->addWidget(nameLabel);
  typeLayout->addWidget(mNameEdit);
  typeLayout->addStretch(1);

  QGroupBox* generalBox = new QGroupBox(QString("&General"));
  generalBox->setLayout(typeLayout);

  mPreLabel = new QLabel();
  mPreLabel->setMinimumWidth(70);
  mPreLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mDateTimeEdit = new QDateTimeEdit();
  mDateTimeEdit->setDateTime(QDateTime::currentDateTime());

  mPostLabel = new QLabel();
  
  QHBoxLayout* dateTimeLayout = new QHBoxLayout();
  dateTimeLayout->addWidget(mPreLabel);
  dateTimeLayout->addWidget(mDateTimeEdit);
  dateTimeLayout->addWidget(mPostLabel, 1);

  QHBoxLayout* weeklyLayout = new QHBoxLayout();
  weeklyLayout->addWidget(new QLabel("on the following days: "));
  for(int i = 0; i < 7; i++) {
    mWeeklyButtons[i] = createWeeklyButton(dayName(i));
    weeklyLayout->addWidget(mWeeklyButtons[i]);
  }
  weeklyLayout->addStretch(1);
  weeklyLayout->setContentsMargins(0, 0, 0, 0);
  mWeeklyWidget = new QWidget();
  mWeeklyWidget->setLayout(weeklyLayout);

  mMessageEdit = new QTextEdit(alarm->message());
  mMessageEdit->setMaximumHeight(70);
  QLabel* messageLabel = new QLabel("Show Message");
  messageLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);

  mAudioFile = new QLabel(alarm->fileName());
  QToolButton* openButton = new QToolButton();
  openButton->setIcon(QIcon(":/icon_open.png"));
  connect(openButton, SIGNAL(clicked()), this, SLOT(openAudioFile()));

  mPlayStopButton = new QToolButton();
  mPlayStopButton->setIcon(QIcon(":/icon_play.png"));
  connect(mPlayStopButton, SIGNAL(clicked()), this, SLOT(playStopClick()));

  QToolButton* mRemoveButton = new QToolButton();
  mRemoveButton->setIcon(QIcon(":/icon_remove.png"));
  connect(mRemoveButton, SIGNAL(clicked()), this, SLOT(removeAudioFile()));

  QHBoxLayout* audioLayout = new QHBoxLayout();
  audioLayout->addWidget(mAudioFile, 1);
  audioLayout->addWidget(openButton);
  audioLayout->addWidget(mPlayStopButton);
  audioLayout->addWidget(mRemoveButton);

  QGridLayout* gridLayout = new QGridLayout();
  gridLayout->addWidget(messageLabel,             0, 0);
  gridLayout->addWidget(mMessageEdit,             0, 1);
  gridLayout->addWidget(new QLabel("Play Sound"), 1, 0);
  gridLayout->addLayout(audioLayout,              1, 1);

  QVBoxLayout* settingsLayout = new QVBoxLayout();
  settingsLayout->addLayout(dateTimeLayout);
  settingsLayout->addWidget(mWeeklyWidget);
  settingsLayout->addStretch(1);
  settingsLayout->addLayout(gridLayout);

  QGroupBox* settingsBox = new QGroupBox(QString("&Settings"));
  settingsBox->setLayout(settingsLayout);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(doAccept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(generalBox);
  layout->addWidget(settingsBox);
  layout->addWidget(buttonBox);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);
  setWindowTitle(QString("Alarm..."));

  mSound = NULL;

  resize(mSettings->value(keySize(), QSize(640, 480)).toSize());
  move(mSettings->value(keyPos(), QApplication::desktop()->screenGeometry().center() - QPoint(640, 480) / 2).toPoint());

  mDateTimeEdit->setDateTime(alarm->time());
  for(unsigned i = 0; i < alarm->weekMask().size(); i++)
    mWeeklyButtons[i]->setChecked(alarm->weekMask(i));

  for(int i = 0; i < mTypeBox->count(); i++) {
    if(mTypeBox->itemData(i).toInt() == alarm->type()) {
      mTypeBox->setCurrentIndex(i);
      break;
    }
  }
}

void AlarmParamsDialog::closeEvent(QCloseEvent *event) {
  mSettings->setValue(keySize(), size());
  mSettings->setValue(keyPos(), pos());
  event->accept();
}

void AlarmParamsDialog::typeIndexChanged(int index) {
  Alarm::Type type = static_cast<Alarm::Type>(mTypeBox->itemData(index).toInt());
  
  if(type == Alarm::ALARM_WEEKLY)
    mWeeklyWidget->show();
  else
    mWeeklyWidget->hide();

  mDateTimeEdit->setMaximumDate(QDate(7999, 12, 31));
  mDateTimeEdit->setMinimumDate(QDate(1752,  9, 14));

  if(type == Alarm::ALARM_COUNTDOWN)
    mDateTimeEdit->setDateTime(QDateTime(QDate::currentDate()));

  switch(type) {
  case Alarm::ALARM_COUNTDOWN:
    mPreLabel->setText("Run after");
    mDateTimeEdit->setDisplayFormat("hh:mm:ss");
    mPostLabel->setText("");
    break;
  case Alarm::ALARM_ONCE:
    mPreLabel->setText("Run once at");
    mDateTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    mPostLabel->setText("");
    break;
  case Alarm::ALARM_MINUTELY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("ss");
    mPostLabel->setText("seconds every minute");
    break;
  case Alarm::ALARM_HOURLY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("mm:ss");
    mPostLabel->setText("every hour");
    break;
  case Alarm::ALARM_DAYLY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("hh:mm:ss");
    mPostLabel->setText("every day");
    break;
  case Alarm::ALARM_WEEKLY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("hh:mm:ss");
    mPostLabel->setText("every week");
    break;
  case Alarm::ALARM_MONTHLY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("dd hh:mm:ss");
    mPostLabel->setText("every month");
    break;
  case Alarm::ALARM_ANNUALLY:
    mPreLabel->setText("Run at");
    mDateTimeEdit->setDisplayFormat("dd.MM hh:mm:ss");
    mPostLabel->setText("every year");
    break;
  }
}

void AlarmParamsDialog::doAccept() {
  Alarm::Type type = static_cast<Alarm::Type>(mTypeBox->itemData(mTypeBox->currentIndex()).toInt());
  QDateTime time = mDateTimeEdit->dateTime();
  if(type == Alarm::ALARM_COUNTDOWN) {
    mAlarm->setType(Alarm::ALARM_ONCE);
    
    QDateTime now = QDateTime::currentDateTime();
    now = now.addSecs(time.time().second() + 60 * time.time().minute() + 60 * 60 * time.time().hour());
    time = now;
  }

  mAlarm->setType(type);
  mAlarm->setTime(time);
  mAlarm->setMessage(mMessageEdit->toPlainText());
  mAlarm->setFileName(mAudioFile->text());
  mAlarm->setName(mNameEdit->text());

  for(int i = 0; i < 7; i++)
    mAlarm->setWeekMask(i, mWeeklyButtons[i]->isChecked());

  mAlarm->recalculateNextRunTime();

  accept();
}

void AlarmParamsDialog::openAudioFile() {
  if(mSound != NULL) {
    delete mSound;
    mSound = NULL;
    mPlayStopButton->setIcon(QIcon(":/icon_play.png"));
  }
  QString fileName = QFileDialog::getOpenFileName(this, "Open Audio File...", 
    mSettings->value(keyLastDir()).toString(), "Audio Files (*.wav *.mp3);;All Files (*.*)"); 
  if(!fileName.isEmpty()) {
    mAudioFile->setText(fileName);
    mSettings->setValue(keyLastDir(), fileName);
  }
}

void AlarmParamsDialog::removeAudioFile() {
  if(mSound != NULL) {
    delete mSound;
    mSound = NULL;
  }
  
  mPlayStopButton->setIcon(QIcon(":/icon_play.png"));
  mAudioFile->setText("");
}

void AlarmParamsDialog::playStopClick() {
  if(mSound == NULL) {
    if(!QFile::exists(mAudioFile->text()))
      return;

    mSound = new Sound(mAudioFile->text(), this); 
    mSound->play();
    mPlayStopButton->setIcon(QIcon(":/icon_stop.png"));
  } else {
    delete mSound;
    mSound = NULL;
    mPlayStopButton->setIcon(QIcon(":/icon_play.png"));
  }
}
