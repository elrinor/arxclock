#include "config.h"
#include <boost/preprocessor/cat.hpp>
#include <QApplication>
#include <QSettings>
#include "Daemon.h"

#ifdef ARXCLOCK_STATIC_PLUGINS
#  include <QtPlugin>
#  ifdef Q_OS_WIN
#    define PHONON_BACKEND phonon_ds9
#    pragma comment(lib, "opengl32.lib")
#    pragma comment(lib, "strmiids.lib")
#    pragma comment(lib, "dmoguids.lib")
#    pragma comment(lib, "msdmo.lib")
Q_IMPORT_PLUGIN(phonon_ds9)
#  endif
#  include <phonon/private/factory_p.h>
#endif

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setQuitOnLastWindowClosed(false);
  app.setWindowIcon(QIcon(":/icon_clock.png"));

#ifdef ARXCLOCK_STATIC_PLUGINS
  Phonon::Factory::setBackend(BOOST_PP_CAT(qt_plugin_instance_, PHONON_BACKEND)());
#endif

  QSettings settings("arxclock.ini", QSettings::IniFormat);
  Daemon daemon(&settings);
  return app.exec();
}