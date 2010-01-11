#ifndef __ARXCLOCK_ALARM_MANAGER_H__
#define __ARXCLOCK_ALARM_MANAGER_H__

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <QHash>
#include "Alarm.h"

// -------------------------------------------------------------------------- //
// AlarmManagerKeys
// -------------------------------------------------------------------------- //
template<class T>
class AlarmManagerKeys {
public:
  typedef T tag;

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
  AlarmManager(QSettings* settings): mSettings(QSettings) {
    rollback();
  }

  const QList<Alarm*>& alarms() const {
    return mAlarms.values();
  }

  Alarm* alarmById(QString id) const {
    return mAlarms[id];
  }

  /** This one also deletes given alarm. */
  void remove(Alarm* alarm) const {
    assert(mAlarms.contains(alarm->id()));

    mAlarms.remove(alarm->id());
    delete alarm;

    mStructurallyModified = true;
  }

  /** This one claims given alarm. */
  void add(Alarm* alarm) const {
    assert(!mAlarms.contains(alarm->id()));

    alarm->setChanged(true);
    mAlarms.insert(alarm->id(), alarm);

    mStructurallyModified = true;
  }

  void commit() {
    foreach(Alarm* alarm, mAlarms) {
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

    foreach(Alarm* alarm, mDeletedAlarms)
      delete alarm;

    if(mStructurallyModified) {
      QList<QVariant> alarmIds;
      foreach(Alarm* alarm, mAlarms)
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

      Alarm* alarm = new Alarm(
        id,
        static_cast<Alarm::Type>(mSettings->value(keyAlarmType(id), 0).toInt()),
        mSettings->value(keyAlarmEnabled(id), true).toBool(),
        mSettings->value(keyAlarmName(id), "Alarm " + id).toString(),
        mSettings->value(keyAlarmMessage(id), QString("Time to watch anime!")).toString(),
        mSettings->value(keyAlarmTime(id), QDateTime::currentDateTime()).toDateTime(),
        mSettings->value(keyAlarmWeekMask(id), QList<QVariant>()).toList(),
        mSettings->value(keyAlarmFileName(id), QString()).toString(),
        mSettings->value(keyAlarmCommandLine(id), QString()).toString()
      );

      mAlarms.insert(id, alarm);
    }

    mStructurallyModified = false;
  }

  void clear() {
    mStructurallyModified = !mAlarms.empty();

    foreach(Alarm* alarm, mAlarms)
      delete alarm;
    mAlarms.clear();
  }

private:
  QHash<QString, Alarm*> mAlarms;
  bool mStructurallyModified;
  QSettings* mSettings;
};


#endif // __ARXCLOCK_ALARM_MANAGER_H__
