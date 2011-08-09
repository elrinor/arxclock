#ifndef ARXCLOCK_ALARM_H
#define ARXCLOCK_ALARM_H

#include "config.h"
#include <cassert>
#include <boost/noncopyable.hpp>
#include <boost/array.hpp>
#include <QCoreApplication> /* For Q_DECLARE_TR_FUNCTIONS. */
#include <QString>
#include <QStandardItem>
#include <QDateTime>
#include "Utility.h"

namespace arxclock {
// -------------------------------------------------------------------------- //
// Alarm
// -------------------------------------------------------------------------- //
  class Alarm: public boost::noncopyable {
    Q_DECLARE_TR_FUNCTIONS(Alarm)
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

    Alarm(const QString& id, Type type, bool enabled, const QString& name, const QString& message, const QDateTime& time, const boost::array<bool, 7>& weekMask, const QString& fileName, const QString& commandLine):
      mId(id), mType(type), mEnabled(enabled), mName(name), mMessage(message), mTime(time), mFileName(fileName), mCommandLine(commandLine), mWeekMask(weekMask), mChanged(false), mActive(false) {
      recalculateNextRunTime();
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

    const boost::array<bool, 7>& weekMask() const {
      return mWeekMask;
    }

    bool weekMask(int index) const {
      return mWeekMask[index];
    }

    const QString& fileName() const {
      return mFileName;
    }

    const QString& commandLine() const {
      return mCommandLine;
    }

    void setType(Type type) {
      assert(type > ALARM_COUNTDOWN && type <= ALARM_LAST_TYPE);

      mChanged = true;
      mType = type;
    }

    void setEnabled(bool enabled)  {
      mChanged = true;
      mEnabled = enabled;
    }

    void setName(const QString& name) {
      mChanged = true;
      mName = name;
    }

    void setMessage(const QString& message) {
      mChanged = true;
      mMessage = message;
    }

    void setTime(const QDateTime& time) {
      mChanged = true;
      mTime = time;
    }

    void setWeekMask(int index, bool value) {
      mChanged = true;
      mWeekMask[index] = value;
    }

    void setFileName(const QString& fileName) {
      mChanged = true;
      mFileName = fileName;
    }

    void setCommandLine(const QString& commandLine) {
      mChanged = true;
      mCommandLine = commandLine;
    }

    QDateTime nextRunTime() const {
      return mNextRunTime;
    }

    bool active() const {
      return mActive;
    }

    static QString typeToString(Type type) {
      switch(type) {
      case ALARM_COUNTDOWN: return tr("Countdown");
      case ALARM_ONCE:      return tr("Once");
      case ALARM_MINUTELY:  return tr("Minutely");
      case ALARM_HOURLY:    return tr("Hourly");
      case ALARM_DAYLY:     return tr("Daily");
      case ALARM_WEEKLY:    return tr("Weekly");
      case ALARM_MONTHLY:   return tr("Monthly");
      case ALARM_ANNUALLY:  return tr("Annually");
      default:              Unreachable();
      }
    }

    QString typeString() const {
      return typeToString(type());
    }

    QString toString() const {
      QString s = typeString();
      switch (mType) {
      case ALARM_ONCE:      return tr("%1 on %2 at %3").arg(s).arg(mTime.toString(tr("dd.MM.yy"))).arg(mTime.toString(tr("hh:mm:ss", "ALARM_ONCE")));
      case ALARM_MINUTELY:  return tr("%1 at %2 seconds").arg(s).arg(mTime.toString(tr("ss")));
      case ALARM_HOURLY:    return tr("%1 at %2", "ALARM_HOURLY").arg(s).arg(mTime.toString(tr("mm:ss")));
      case ALARM_DAYLY:     return tr("%1 at %2", "ALARM_DAYLY").arg(s).arg(mTime.toString(tr("hh:mm:ss", "ALARM_DAYLY")));
      case ALARM_WEEKLY: { 
        QString dayList;
        if(mNextRunTime == never()) {
          dayList = tr("<never>");
        } else {
          bool comma = false;
          for(int i = 0; i < 7; i++) {
            if(mWeekMask[i]) {
              if(comma)
                dayList += ", ";
              dayList += dayName(i);
              comma = true;
            }
          }
        }
        return tr("%1 on %2 at %3").arg(s).arg(dayList).arg(mTime.toString(tr("hh:mm:ss", "ALARM_WEEKLY")));
      }
      case ALARM_MONTHLY:   return tr("%1 on %2 day of the month at %3").arg(s).arg(mTime.toString(tr("dd"))).arg(mTime.toString(tr("hh:mm:ss", "ALARM_MONTHLY")));
      case ALARM_ANNUALLY:  return tr("%1 on %2 at %3").arg(s).arg(mTime.toString(tr("dd.MM"))).arg(mTime.toString(tr("hh:mm:ss", "ALARM_ANNUALLY")));
      default:              Unreachable();
      }
    }

    void setActive(bool active) {
      mActive = active;
    }

    void setNextRunTime(const QDateTime& time) {
      mNextRunTime = time;
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
        for(unsigned i = 0; i < mWeekMask.size(); i++) {
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

    static QDateTime never() {
      return QDateTime(QDate(2999, 12, 31));
    }

  protected:
    bool changed() const {
      return mChanged;
    }

    void setChanged(bool changed) {
      mChanged = changed;
    }

    friend class AlarmManager;

  private:
    /* Saved state. */
    QString mId;
    Type mType;
    bool mEnabled;
    QString mName;
    QString mMessage;
    QDateTime mTime;
    boost::array<bool, 7> mWeekMask;
    QString mFileName;
    QString mCommandLine;

    /* In-app state. */
    bool mChanged;
    bool mActive;
    QDateTime mNextRunTime;
  };

} // namespace arxclock

#endif // ARXCLOCK_ALARM_H
