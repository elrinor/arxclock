TEMPLATE  = app
CONFIG   += qt warn_on
#CONFIG   += embed_manifest_exe
#CONFIG   += static

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
MOC_DIR   = src/moc
RCC_DIR   = src/rcc
TARGET    = arxclock

win32:RC_FILE             = src/res/arxclock.rc

win32:debug:DESTDIR       = bin/debug
win32:debug:OBJECTS_DIR   = bin/debug
win32:release:DESTDIR     = bin/release
win32:release:OBJECTS_DIR = bin/release

win32:debug:CONFIG       += console

win32:release:CONFIG     -= console

win32:release:QMAKE_POST_LINK = upx -9 -q $$DESTDIR/$$join(TARGET, "", "", ".exe")
