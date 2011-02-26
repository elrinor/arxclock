#ifndef ARXCLOCK_UTILITY_H
#define ARXCLOCK_UTILITY_H

#include "config.h"
#include <cassert>
#include <QString>

#define Unreachable() {assert(!"Unreachable code executed."); throw 0;} 

namespace arxclock {

  namespace detail {
    static const QString dayNames[] = {
      QString("Mon"), 
      QString("Tue"), 
      QString("Wed"), 
      QString("Thu"), 
      QString("Fri"), 
      QString("Sat"), 
      QString("Sun")
    };

  } // namespace detail

  static QString dayName(int dayOfTheWeek) {
    assert(dayOfTheWeek >= 0 && dayOfTheWeek < 7);

    return detail::dayNames[dayOfTheWeek];
  }

} // namespace arxclock

#endif // ARXCLOCK_UTILITY_H
