TEMPLATE  = app
CONFIG   += qt warn_on
QT       += phonon

SOURCES = \
  src/arxclock.cpp \
  src/arxclock/AlarmParamsDialog.cpp \
  src/arxclock/Daemon.cpp \
  src/arxclock/MainWidget.cpp \
  src/arxclock/RingWidget.cpp \
  src/arxclock/SnoozeDialog.cpp \
  src/arxclock/Sound.cpp \
  
HEADERS = \
  src/config.h \
  src/arxclock/Alarm.h \
  src/arxclock/AlarmParamsDialog.h \
  src/arxclock/Daemon.h \
  src/arxclock/MainWidget.h \
  src/arxclock/RingWidget.h \
  src/arxclock/SnoozeDialog.h \
  src/arxclock/Sound.h \  

FORMS = \
  src/arxclock/AlarmParamsDialog.ui \
  src/arxclock/MainWidget.ui \
  src/arxclock/RingWidget.ui \
  src/arxclock/SnoozeDialog.ui \

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
