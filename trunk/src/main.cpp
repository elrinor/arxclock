#include "config.h"
#include <QApplication>
#include <QtCore>
#include "Daemon.h"
#include "Sound.h"

/* TODO:
bass better build-in*/

int main(int argc, char *argv[]) {
  Sound::init();
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  app.setWindowIcon(QIcon(":/icon_clock.png"));
  
  QSettings settings("[ArX]Team", "arxclock");
  Daemon daemon(&settings);
  return app.exec();
}