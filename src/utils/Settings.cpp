#include <QSettings>
#include <QString>
#include <QVariant>
#include <cstring>

#include "../tools.h"

QSettings* settings;
QSettings* settingsDefault;
void settings_init() {
    QString settingsFile = QString(getenv("HOME"))+"/.config/pardus-pen.conf";
    settings = new QSettings(settingsFile, QSettings::NativeFormat);
    settingsDefault = new QSettings("://tr.org.pardus.pen.default.conf", QSettings::NativeFormat);
}

QString get_string(const char* name) {
    QString value;
    if(settings->contains(QString::fromUtf8(name))){
        value = settings->value(QString::fromUtf8(name)).toString();
    } else {
        value = settingsDefault->value(QString::fromUtf8(name)).toString();
    }
    //printf("GETSTRING %s:%s\n", name, value.toUtf8().constData());
    return value;
}

bool get_bool(const char* name) {
    bool value;
    if(settings->contains(QString::fromUtf8(name))){
        value = settings->value(QString::fromUtf8(name)).toBool();
    } else {
        value = settingsDefault->value(QString::fromUtf8(name)).toBool();
    }
    //printf("GETBOOL %s:%d\n", name, value);
    return value;
}

void set_bool(const char* name, bool value) {
    settings->setValue(QString::fromUtf8(name), value);
    //printf("SETBOOL %s:%d\n", name, value);
    settings->sync();
}

void set_string(const char* name, QString value) {
    settings->setValue(QString::fromUtf8(name), value);
    //printf("SETSTRING %s:%s\n", name, value);
    settings->sync();
}

int get_int(const char* name) {
    int value;
    if(settings->contains(QString::fromUtf8(name))){
        value = settings->value(QString::fromUtf8(name)).toInt();
    } else {
        value = settingsDefault->value(QString::fromUtf8(name)).toInt();
    }
    //printf("GETINT %s:%d\n", name, value);
    return value;
}

void set_int(const char* name, int value) {
    settings->setValue(QString::fromUtf8(name), value);
    //printf("SETINT %s:%d\n", name, value);
    settings->sync();
}

