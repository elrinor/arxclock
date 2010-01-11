#ifndef __ARXCLOCK_ALARM_H__
#define __ARXCLOCK_ALARM_H__

#include "config.h"
#include <boost/noncopyable.hpp>
#include <QtCore>
#include <QStandardItem>
#include "Utility.h"

// -------------------------------------------------------------------------- //
// Alarm
// -------------------------------------------------------------------------- //
class Alarm: public boost::noncopyable {
public:
  enum Type {
    ALARM_COUNTDOWN = 0,
    ALARM_FIRST_TYPE = ALARM_COUNTDOWN,
    ALARM_ONCE,
    ALARM_MINUTELY,
    ALARM_HOURLY,
    ALARM_DAYLY,
    ALARM_WEEKLY,
    ALARM_MONTHLY,
    ALARM_ANNUALLY,
    ALARM_LAST_TYPE = ALARM_ANNUALLY
  };

  Alarm::Alarm(const QString& id, Type type, bool enabled, const QString& name, const QString& message, const QDateTime& time, const QList<QVariant>& weekMask, const QString& fileName, const QString& commandLine):
    mId(id), mType(type), mEnabled(enabled), mName(name), mMessage(message), mTime(time), mFileName(fileName), mCommandLine(commandLine) {
    foreach(const QVariant& value, weekMask)
      mWeekMask.append(value.toBool());

    mChanged = false;
    //this->mId = now.toString("hh_mm_ss_zzz") + "_" + QString().setNum(qrand());
  }

  const QString& id() const {
    return mId;
  }

  Type type() const {
    return mType;
  }

  bool enabled() const {
    return mEnabled;
  }

  const QString& name() const {
    return mName;
  }

  const QString& message() const {
    return mMessage;
  }

  const QDateTime& time() const {
    return mTime;
  }

  const QList<bool>& weekMask() const {
    return mWeekMask;
  }

  const QString& fileName() const {
    return mFileName;
  }

  const QString& commandLine() const {
    return mCommandLine;
  }

  QDateTime nextRunTime() const {
    return mNextRunTime;
  }

  static QString typeToString(Type type) {
    switch(type) {
    case ALARM_COUNTDOWN: return "Countdown";
    case ALARM_ONCE:      return "Once";
    case ALARM_MINUTELY:  return "Minutely";
    case ALARM_HOURLY:    return "Hourly";
    case ALARM_DAYLY:     return "Daily";
    case ALARM_WEEKLY:    return "Weekly";
    case ALARM_MONTHLY:   return "Monthly";
    case ALARM_ANNUALLY:  return "Annually";
    default:              Unreachable();
    }
  }

  QString typeString() const {
    return typeToString(type());
  }

  QString toString() const {
    QString s = typeString();
    switch (mType) {
    case ALARM_ONCE:      return s + " on " + mTime.toString("dd.MM.yy") + " at " + mTime.toString("hh:mm:ss");
    case ALARM_MINUTELY:  return s + " at " + mTime.toString("ss") + " seconds";
    case ALARM_HOURLY:    return s + " at " + mTime.toString("mm:ss");
    case ALARM_DAYLY:     return s + " at " + mTime.toString("hh:mm:ss");
    case ALARM_WEEKLY:    
      s += " on ";
      if(mNextRunTime == never()) {
        s += "<never>";
      } else {
        bool comma = false;
        for(int i = 0; i < 7; i++) {
          if(mWeekMask[i]) {
            if(comma)
              s += ", ";
            s += dayName(i);
            comma = true;
          }
        }
      }
      s += " at " + mTime.toString("hh:mm:ss");
      return s;
    case ALARM_MONTHLY:   return s + " on " + mTime.toString("dd") + " day of the month at " + mTime.toString("hh:mm:ss");
    case ALARM_ANNUALLY:  return s + " on " + mTime.toString("dd.MM") + " at " + mTime.toString("hh:mm:ss");
    default:              Unreachable();
    }
  }

  void recalculateNextRunTime() {
    QDateTime now = QDateTime::currentDateTime();
    
    switch(mType) {
    case ALARM_ONCE: 
      mNextRunTime = now >= mTime ? never() : mTime;
      break;
    case ALARM_MINUTELY: 
      if(now.time().second() >= mTime.time().second())
        now = now.addSecs(60);
      mNextRunTime = QDateTime(now.date(), QTime(now.time().hour(), now.time().minute(), mTime.time().second()));
      break;
    case ALARM_HOURLY:
      if(now.time().minute() * 60 + now.time().second() >= 
        mTime.time().minute() * 60 + mTime.time().second())
        now = now.addSecs(60 * 60);
      mNextRunTime = QDateTime(now.date(), QTime(now.time().hour(), mTime.time().minute(), mTime.time().second()));
      break;
    case ALARM_DAYLY:
      if(now.time() >= mTime.time())
        now = now.addDays(1);
      mNextRunTime = QDateTime(now.date(), mTime.time());
      break;
    case ALARM_WEEKLY: {
      bool isNever = true;
      for(int i = 0; i < mWeekMask.size(); i++) {
        if(mWeekMask[i]) {
          isNever = false;
          break;
        }
      }
      if(isNever) {
        mNextRunTime = never();
        break;
      }
      if(now.time() >= mTime.time())
        now = now.addDays(1);
      while(!mWeekMask[now.date().dayOfWeek() - 1])
        now = now.addDays(1);
      mNextRunTime = QDateTime(now.date(), mTime.time());
      break;
    }
    case ALARM_MONTHLY:
      if(now.date().day() > mTime.date().day() || 
        (now.date().day() == mTime.date().day() && now.time() >= mTime.time()))
        now = now.addMonths(1);
      mNextRunTime = QDateTime(QDate(now.date().year(), now.date().month(), mTime.date().day()), mTime.time());
      break;
    case ALARM_ANNUALLY:
      if(now.date().month() * 100 + now.date().day() > mTime.date().month() * 100 + mTime.date().day() || 
        (now.date().month() * 100 + now.date().day() == mTime.date().month() * 100 + mTime.date().day() && 
        now.time() >= mTime.time()))
        now = now.addYears(1);
      mNextRunTime = QDateTime(QDate(now.date().year(), mTime.date().month(), mTime.date().day()), mTime.time());
      break;
    default:
      Unreachable();
    }
  }

  //QList<QStandardItem*> toRow() const;

  QString toString() const;

protected:
  bool changed() const {
    return mChanged;
  }

  void setChanged(bool changed) {
    mChanged = changed;
  }

private:
  static QDateTime never() {
    return QDateTime(QDate(7999, 12, 31));
  }

  /* Saved state. */
  QString mId;
  Type mType;
  bool mEnabled;
  QString mName;
  QString mMessage;
  QDateTime mTime;
  QList<bool> mWeekMask;
  QString mFileName;
  QString mCommandLine;

  /* In-app state. */
  bool mChanged;
  QDateTime mNextRunTime;
};

#endif // __ARXCLOCK_ALARM_H__
