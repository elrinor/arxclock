#ifndef __ARXCLOCK_CONFIG_H__
#define __ARXCLOCK_CONFIG_H__

#define ARXCLOCK_VERSION "1.0.1"

#define KEY_SETTINGDLG_POS  (QString("main_dialog/position"))
#define KEY_SETTINGDLG_SIZE (QString("main_dialog/size"))

#define KEY_RINGDLG_POS     (QString("ring_dialog/position"))
#define KEY_RINGDLG_SIZE    (QString("ring_dialog/size"))

#define KEY_ALARMDLG_POS    (QString("alarm_dialog/position"))
#define KEY_ALARMDLG_SIZE   (QString("alarm_dialog/size"))
#define KEY_LAST_OPENDIR    (QString("last_open_dir"))

#define Unreachable() {assert(!"Unreachable code executed."); throw 0;} 

#endif // __ARXCLOCK_CONFIG_H__
