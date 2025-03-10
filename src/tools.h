
#include "widgets/DrawingWidget.h"
#include "widgets/FloatingWidget.h"
#include "widgets/FloatingSettings.h"
#include "widgets/WhiteBoard.h"
#include "widgets/Background.h"
#include "widgets/Button.h"
#include "widgets/OverView.h"

#include "utils/ScreenShot.h"

#include "constants.h"

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
    #include "utils/Settings.h"
}
extern QPushButton **penButtons;

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

extern void updateRatioButtons();
extern void updateGoBackButtons();
extern void backgroundStyleEvent();
extern void penStyleEvent();
extern void penSizeEvent();

extern void setPen(int type);
extern void setPenStyle(int style);
extern void setLineStyle(int style);

extern QPushButton **penButtons;

extern QPushButton *typeButton;

extern OverView *ov;

extern QPushButton *backgroundButton;
extern QPushButton* minify;
extern QPushButton* rotate;
extern QPushButton* fullscreen;

extern QPushButton* colorpicker;

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
