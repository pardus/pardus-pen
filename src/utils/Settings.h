#include <stdbool.h>
#ifndef _SETTINGS_H
#define _SETTINGS_H
void settings_init();
char* get_string(char* name);
void set_string(char* name, char* value);
int get_int(char* name);
void set_int(char* name, int value);
bool get_bool(char* name);
void set_bool(char* name, bool value);

#endif
