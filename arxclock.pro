TEMPLATE  = app
CONFIG   += qt warn_on

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

FORMS = 

RESOURCES = \
  src/res/arxclock.qrc \

win32:LIBS += lib/bass.lib

UI_DIR    = src/ui
MOC_DIR   = bin/temp/moc
RCC_DIR   = bin/temp/rcc
TARGET    = arxclock

win32:RC_FILE    = src/res/arxclock.rc

CONFIG(debug, debug|relase) {
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
