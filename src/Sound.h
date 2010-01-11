#ifndef __ARXCLOCK_SOUND_H__
#define __ARXCLOCK_SOUND_H__

#include "config.h"
#include <QObject>
#include <QString>

class Sound: public QObject {
  Q_OBJECT;

private:
  struct SoundImpl;
  SoundImpl* impl;

public:
  static void init();

  Sound(QString fileName, QObject* parent = NULL);

  void play();
  void stop();

  ~Sound();
};

#endif // __ARXCLOCK_SOUND_H__
