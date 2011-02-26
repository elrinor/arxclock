#ifndef ARXCLOCK_SOUND_H
#define ARXCLOCK_SOUND_H

#include "config.h"
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace arxclock {
// -------------------------------------------------------------------------- //
// Sound
// -------------------------------------------------------------------------- //
  class Sound: public QObject {
    Q_OBJECT;
  public:
    Sound(QString fileName, QObject *parent = NULL);

    void play();
    void stop();

    ~Sound();

  private:
    struct SoundImpl;
    QScopedPointer<SoundImpl> mImpl;
  };

} // namespace arxclock

#endif // ARXCLOCK_SOUND_H
