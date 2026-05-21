#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#ifndef QUNUSED
    #define Q_UNUSED(x) (void)x;
#endif

#ifndef BGDIR
#define BGDIR "/usr/share/pardus/pardus-pen/backgrounds"
#endif

extern float scale;

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

extern "C" {
    extern size_t _cur_time;
    size_t get_epoch();
    char* which(const char* cmd);
    void disable_erc();
    void enable_erc();
}

#define _(String) gettext(String)

/********** Enums **********/

#define ICON 0
#define ERASER 1
#define PEN 2
#define MARKER 3
#define SELECTION 4
#define PENTEXT 5

#define LINE 10
#define CIRCLE 11
#define SPLINE 12
#define RECTANGLE 13
#define TRIANGLE 14
#define VECTOR 15
#define VECTOR2 16

#define NORMAL 20
#define DOTLINE 21
#define LINELINE 22
#define FILLED 23

#define TRANSPARENT 30
#define WHITE 31
#define BLACK 32

#define BLANK 40
#define SQUARES 41
#define LINES 42
#define ISOMETRIC 43
#define MUSIC 44
#define CUSTOM 45

#define COLORPICKER 51
#define BACK 52
#define NEXT 53
#define MINIFY 54
#define FULLSCREEN 55
#define FULLSCREEN_EXIT 56
#define ROTATE 57
#define HIDEUI 58
#define OVERLAYSCALEUP 59
#define OVERLAYSCALEDOWN 60
#define OVERLAYROTATEUP 61
#define OVERLAYROTATEDOWN 62
#define CLEAR 63
#define PREVPAGE 64
#define NEXTPAGE 65
#define CLOSE 66
#define PENMENU 67
#define ERASERMENU 68
#define PAGEMENU 69
#define SAVE 70
#define OPEN 71
#define SCREENSHOT 72
#define UTILMENU 73
#define CURSOR 74
#define SHAPEMENU 75
#define UNMINIFY 76
#define SCREENSHOT_CROP 77
