#ifndef ARXCLOCK_UTILITY_H
#define ARXCLOCK_UTILITY_H

#include "config.h"
#include <cassert>
#include <QCoreApplication> /* For Q_DECLARE_TR_FUNCTIONS. */
#include <QString>

#define Unreachable() {assert(!"Unreachable code executed."); throw 0;} 

namespace arxclock {

  class DayNames {
    Q_DECLARE_TR_FUNCTIONS(DayNames);
  public:
    static inline QString dayName(int dayOfTheWeek) {
      assert(dayOfTheWeek >= 0 && dayOfTheWeek < 7);

      static const char *dayNames[] = {
        QT_TR_NOOP("Mon"), 
        QT_TR_NOOP("Tue"), 
        QT_TR_NOOP("Wed"), 
        QT_TR_NOOP("Thu"), 
        QT_TR_NOOP("Fri"), 
        QT_TR_NOOP("Sat"), 
        QT_TR_NOOP("Sun")
      };

      return tr(dayNames[dayOfTheWeek]);
    }

  };

  inline QString dayName(int dayOfTheWeek) {
    return DayNames::dayName(dayOfTheWeek);
  }

} // namespace arxclock

#endif // ARXCLOCK_UTILITY_H
