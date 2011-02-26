#ifndef ARXCLOCK_ALARM_MANAGER_H
#define ARXCLOCK_ALARM_MANAGER_H

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <QHash>
#include "Alarm.h"

// -------------------------------------------------------------------------- //
// AlarmManagerKeys
// -------------------------------------------------------------------------- //
class AlarmManagerKeys {
public:
  static QString keyAlarmIds        ()                  { return "alarm_ids"; }
  static QString keyAlarmEnabled    (const QString& id) { return "alarm_" + id + "/enabled"; }
  static QString keyAlarmName       (const QString& id) { return "alarm_" + id + "/name"; }
  static QString keyAlarmMessage    (const QString& id) { return "alarm_" + id + "/message"; }
  static QString keyAlarmType       (const QString& id) { return "alarm_" + id + "/type"; }
  static QString keyAlarmTime       (const QString& id) { return "alarm_" + id + "/time"; }
  static QString keyAlarmWeekMask   (const QString& id) { return "alarm_" + id + "/week_mask"; }
  static QString keyAlarmFileName   (const QString& id) { return "alarm_" + id + "/file_name"; }
  static QString keyAlarmCommandLine(const QString& id) { return "alarm_" + id + "/command_line"; }
};


// -------------------------------------------------------------------------- //
// AlarmManager
// -------------------------------------------------------------------------- //
class AlarmManager: public boost::noncopyable, private AlarmManagerKeys {
public:
  AlarmManager(QSettings *settings): mSettings(settings) {
    rollback();
  }

  const QList<boost::shared_ptr<Alarm> > alarms() const {
    return mAlarms;
  }

  boost::shared_ptr<Alarm> alarmById(QString id) const {
    return mAlarmsMap[id];
  }

  void remove(boost::shared_ptr<Alarm> alarm) {
    assert(mAlarmsMap.contains(alarm->id()));

    mAlarms.removeAll(alarm);
    mAlarmsMap.remove(alarm->id());

    mStructurallyModified = true;
  }

  void add(boost::shared_ptr<Alarm> alarm) {
    assert(!mAlarmsMap.contains(alarm->id()));

    alarm->setChanged(true);
    mAlarms.append(alarm);
    mAlarmsMap.insert(alarm->id(), alarm);

    mStructurallyModified = true;
  }

  void commit() {
    foreach(boost::shared_ptr<Alarm> alarm, mAlarms) {
      if(alarm->changed()) {
        QString id = alarm->id();

        mSettings->setValue(keyAlarmType(id), static_cast<int>(alarm->type()));
        mSettings->setValue(keyAlarmEnabled(id), alarm->enabled());
        mSettings->setValue(keyAlarmName(id), alarm->name());
        mSettings->setValue(keyAlarmMessage(id), alarm->message());
        mSettings->setValue(keyAlarmTime(id), alarm->time());

        QList<QVariant> weekMask;
        foreach(bool bit, alarm->weekMask())
          weekMask.append(bit);
        mSettings->setValue(keyAlarmWeekMask(id), weekMask);

        mSettings->setValue(keyAlarmFileName(id), alarm->fileName());
        mSettings->setValue(keyAlarmCommandLine(id), alarm->commandLine());

        alarm->setChanged(false);
      }
    }

    if(mStructurallyModified) {
      QList<QVariant> alarmIds;
      foreach(boost::shared_ptr<Alarm> alarm, mAlarms)
        alarmIds.append(alarm->id());
      mSettings->setValue(keyAlarmIds(), alarmIds);

      mStructurallyModified = false;
    }
  }

  void rollback() {
    clear();

    QList<QVariant> alarmIds = mSettings->value(keyAlarmIds(), QList<QVariant>()).toList();
    foreach(const QVariant& variantId, alarmIds) {
      QString id = variantId.toString();

      boost::array<bool, 7> weekMask;
      std::fill(weekMask.begin(), weekMask.end(), false);
      
      QList<QVariant> variantWeekMask = mSettings->value(keyAlarmWeekMask(id), QList<QVariant>()).toList();
      for(int i = 0; i < std::min(variantWeekMask.size(), static_cast<int>(weekMask.size())); i++)
        weekMask[i] = variantWeekMask[i].toBool();

      boost::shared_ptr<Alarm> alarm(new Alarm(
        id,
        static_cast<Alarm::Type>(mSettings->value(keyAlarmType(id), 0).toInt()),
        mSettings->value(keyAlarmEnabled(id), true).toBool(),
        mSettings->value(keyAlarmName(id), "Alarm " + id).toString(),
        mSettings->value(keyAlarmMessage(id), QString("Time to watch anime!")).toString(),
        mSettings->value(keyAlarmTime(id), QDateTime::currentDateTime()).toDateTime(),
        weekMask,
        mSettings->value(keyAlarmFileName(id), QString()).toString(),
        mSettings->value(keyAlarmCommandLine(id), QString()).toString()
      ));

      mAlarms.append(alarm);
      mAlarmsMap.insert(id, alarm);
    }

    mStructurallyModified = false;
  }

  void clear() {
    mStructurallyModified = !mAlarms.empty();
    mAlarms.clear();
    mAlarmsMap.clear();
  }

private:
  QHash<QString, boost::shared_ptr<Alarm> > mAlarmsMap;
  QList<boost::shared_ptr<Alarm> > mAlarms;
  bool mStructurallyModified;
  QSettings *mSettings;
};


#endif // ARXCLOCK_ALARM_MANAGER_H
