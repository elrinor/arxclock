#ifndef __ARXCLOCK_UTILITY_H__
#define __ARXCLOCK_UTILITY_H__

#include <cassert>
#include <QString>

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
}

static QString dayName(int dayOfTheWeek) {
  assert(dayOfTheWeek >= 0 && dayOfTheWeek < 7);

  return detail::dayNames[dayOfTheWeek];
}

#endif // __ARXCLOCK_UTILITY_H__
