TEMPLATE  = app
CONFIG   += qt warn_on
QT       += phonon

SOURCES = \
  src/AlarmParamsDialog.cpp \
  src/Daemon.cpp \
  src/main.cpp \
  src/MainWidget.cpp \
  src/RingWidget.cpp \
  src/Sound.cpp \
  
HEADERS = \
  src/Alarm.h \
  src/AlarmParamsDialog.h \
  src/config.h \
  src/Daemon.h \
  src/MainWidget.h \
  src/RingWidget.h \
  src/SnoozeDialog.h \
  src/Sound.h \  

FORMS = \
  src/AlarmParamsDialog.ui \
  src/MainWidget.ui \
  src/RingWidget.ui \

RESOURCES = \
  res/arxclock.qrc \

UI_DIR    = bin/temp/ui
MOC_DIR   = bin/temp/moc
RCC_DIR   = bin/temp/rcc
TARGET    = arxclock

win32:RC_FILE    = res/arxclock.rc

CONFIG(debug, debug|release) {
  CONFIG           += console
  win32 {
    DESTDIR         = bin/debug
    OBJECTS_DIR     = bin/debug
  }
}

CONFIG(release, debug|release) {
  CONFIG           -= console
  win32 {
    DESTDIR         = bin/release
    OBJECTS_DIR     = bin/release
    QMAKE_POST_LINK = upx -9 -q $$DESTDIR/$$join(TARGET, "", "", ".exe")
  }
}

contains(CONFIG, static) {
  QTPLUGIN += phonon_ds9
  DEFINES  += ARXCLOCK_STATIC_PLUGINS
}
