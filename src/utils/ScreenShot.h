#ifdef screenshot
#ifndef _screenshot_h
#define _screenshot_h

#include <QString>
#include <QStandardPaths>
#include <QStandardPaths>
#include <QDateTime>
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QPixmap>
#include <QFile>

#ifdef DBUS
extern "C" char* screenshot_xdg_portal();
#endif

void takeScreenshot();
#endif
#endif
