#include <stdbool.h>
#ifndef _SETTINGS_H
#define _SETTINGS_H
void settings_init();
QString get_string(const char* name);
void set_string(const char* name, const QString value);
int get_int(const char* name);
void set_int(const char* name, int value);
bool get_bool(const char* name);
void set_bool(const char* name, bool value);

#endif
