#include "bass.h"
#include "Sound.h"

struct Sound::SoundImpl {
  HSTREAM data;
};

void Sound::init() {
  BASS_Init(-1, 44100, 0, 0, 0);
}

Sound::Sound(QString fileName, QObject* parent): QObject(parent) {
  this->impl = new SoundImpl();
  this->impl->data = BASS_StreamCreateFile(false, fileName.toStdWString().c_str(), 0, 0, BASS_UNICODE); 
  BASS_ChannelSetPosition(this->impl->data, 0, BASS_MUSIC_POSRESET);
  BASS_ChannelFlags(this->impl->data, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
}

Sound::~Sound() {
  BASS_StreamFree(this->impl->data);
  delete this->impl;
}

void Sound::play() {
  BASS_Start();
  BASS_ChannelPlay(this->impl->data, 0);
}

void Sound::stop() {
  BASS_Stop();
}