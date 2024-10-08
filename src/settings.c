#include <gio/gio.h>
GSettings* settings;
#include "settings.h"

void settings_init() {
    settings = g_settings_new (SCHEME);
}

char* get_string(char* name){
    return strdup(g_settings_get_string(settings, name));
}

bool get_bool(char* name){
    return g_settings_get_boolean(settings, name);
}
void set_bool(char* name, bool value){
    g_settings_set_boolean(settings, name, value);
    g_settings_sync();
}

void set_string(char* name, char* value) {
    g_settings_set_string(settings, name, value);
    g_settings_sync();
}

int get_int(char* name){
    return g_settings_get_int(settings, name);
}

void set_int(char* name, int value) {
    g_settings_set_int(settings, name, value);
    g_settings_sync();
}

