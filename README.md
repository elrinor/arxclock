# arxclock
ArXClock is a simple alarm clock application written in C++ using Qt and (a bit of) boost libraries. ArXClock uses Qt phonon library for music playback. On Windows, phonon uses DirectSound, so you'll have to install the relevant filters.

Main features of ArXClock are:
  * daily, weekly, monthly alarms;
  * playback of a music file on alarm;
  * execution of a shell command on alarm;
  * snoozing a ringing alarm for an arbitrary amount of time.
