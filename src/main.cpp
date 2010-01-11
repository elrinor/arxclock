#include "config.h"
#include <QApplication>
#include <QtCore>
#include "Daemon.h"
#include "Sound.h"

/* TODO:
when closing edit window we're on a wrong position in a list
silent alarm blocks loud alarm
bass better build-in*/

int main(int argc, char *argv[]) {
  Sound::init();
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  app.setWindowIcon(QIcon(":/icon_clock.png"));
  Daemon daemon;
  return app.exec();
}