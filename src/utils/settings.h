#define SCHEME "tr.org.pardus.pen"
#include <stdbool.h>

void settings_init();
char* get_string(char* name);
void set_string(char* name, char* value);
int get_int(char* name);
void set_int(char* name, int value);
bool get_bool(char* name);
void set_bool(char* name, bool value);
