#include "Sound.h"
#include <phonon/phonon>

namespace arxclock {

  struct Sound::SoundImpl {
    QScopedPointer<Phonon::MediaObject> music;
  };

  Sound::Sound(QString fileName, QObject *parent): QObject(parent) {
    mImpl.reset(new SoundImpl());
    mImpl->music.reset(Phonon::createPlayer(Phonon::MusicCategory, Phonon::MediaSource(fileName)));
  }

  Sound::~Sound() {
    return;
  }

  void Sound::play() {
    mImpl->music->play();
  }

  void Sound::stop() {
    mImpl->music->stop();
  }

} // namespace arxclock
