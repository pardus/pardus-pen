
#include "widgets/DrawingWidget.h"
#include "widgets/FloatingWidget.h"
#include "widgets/FloatingSettings.h"
#include "widgets/WhiteBoard.h"
#include "widgets/Background.h"
#include "widgets/Button.h"
#include "widgets/OverView.h"

#include "utils/ScreenShot.h"

#ifndef BGDIR
#define BGDIR "/usr/share/pardus/pardus-pen/backgrounds"
#endif

#define butsize 48*scale
#define padding 8*scale

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#define _(String) gettext(String)

extern "C" {
#include "utils/settings.h"
}

extern DrawingWidget *drawing;
extern FloatingWidget *floatingWidget;
extern FloatingSettings *floatingSettings;
extern Background *background;

extern WhiteBoard *board;
extern QMainWindow* tool;
extern QMainWindow* tool2;

extern float scale;
extern int history;

extern QSlider *scrollHSlider;
extern QSlider *scrollVSlider;

extern QWidget *mainWidget;

extern void updateGoBackButtons();
extern void backgroundStyleEvent();
extern void penStyleEvent();
extern void penSizeEvent();

extern void setPen(int type);
extern void setPenStyle(int style);
extern void setLineStyle(int style);

extern QPushButton *penButton;
extern QPushButton *selectButton;
extern QPushButton *typeButton;
extern QPushButton *markerButton;
extern QPushButton *eraserButton;
extern QPushButton *splineButton;
extern QPushButton *lineButton;
extern QPushButton *circleButton;
extern QPushButton *triangleButton;
extern QPushButton *rectButton;

extern OverView *ov;

extern QPushButton *lineNormalButton;
extern QPushButton *lineDotLineButton;
extern QPushButton *lineLineLineButton;

extern QPushButton *backgroundButton;
extern QPushButton* minify;
extern QPushButton* rotate;
extern QPushButton* fullscreen;

extern QPushButton* transparentButton;
extern QPushButton* blackButton;
extern QPushButton* whiteButton;
extern QPushButton* colorpicker;

extern QPushButton* overlayIsometric;
extern QPushButton* overlayMusic;
extern QPushButton* overlayCustom;
extern QPushButton* overlayTurkiye;
extern QPushButton* overlayWorld;
extern QPushButton* overlayLines;
extern QPushButton* overlaySquares;
extern QPushButton* overlayNone;

extern QPushButton* overlayScaleUp;
extern QPushButton* overlayScaleDown;

extern QPushButton* overlayRotateUp;
extern QPushButton* overlayRotateDown;

extern QPushButton *backButton;
extern QPushButton *nextButton;

extern QPushButton *previousPage;
extern QPushButton *nextPage;
extern QLabel *pageLabel;

extern QPushButton *clear;
extern QPushButton *close;

extern QWidget *penSettings;
extern QWidget *colorDialog;
extern QWidget *typeDialog;
extern QSlider *thicknessSlider;
extern QLabel *thicknessLabel;
extern QLabel *colorLabel;

extern QWidget *penTypeDialog;
extern QWidget *modeDialog;


extern QLabel *move;

extern QPushButton *save;
extern QPushButton *open;
extern QPushButton *ssButton;

extern "C" {
    extern void *load_archive(void* arg);
    extern QString archive_target;
    extern void settings_init();
}

extern void penSizeEvent();
extern void penStyleEvent();
extern void setupWidgets();

extern bool hasSelection;

extern void setupBackground();
extern void setupScreenShot();
extern void setupPenType();
extern void setupSaveLoad();
extern void backgroundStyleEvent();

extern void setupWidgets();

extern QPushButton *penMenu;
extern QPushButton *penSwitch;

void setHideMainWindow(bool status);
