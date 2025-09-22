
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

#define padding 8*scale

#define butsize (40*scale + padding)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include <stdlib.h>
#include <locale.h>
#include <libintl.h>


extern "C" {
    extern size_t _cur_time;
    size_t get_epoch();
    char* which(const char* cmd);
}


#ifndef DEBUG
#define debug(...)
#else
#define debug printf("[%s:%ld]:", __func__, get_epoch() - _cur_time); _cur_time = get_epoch(); printf
#endif


#define _(String) gettext(String)

#include "utils/Settings.h"

extern QMap<qint64, QPushButton*> penButtons;
extern QMap<qint64, QPushButton*> toolButtons;

extern DrawingWidget *drawing;
extern FloatingWidget *floatingWidget;
extern FloatingWidget *desktopWidget;
extern FloatingSettings *floatingSettings;
extern Background *background;

extern WhiteBoard *board;
extern QMainWindow* tool;
extern QMainWindow* tool2;

extern QWidget *bgMenu;

extern float scale;
extern int history;
extern bool is_wayland;

extern QSlider *scrollHSlider;
extern QSlider *scrollVSlider;

extern QWidget *mainWidget;

extern void updateGui();


extern void setPen(int type);
extern int getPen();
extern void setPenStyle(int style);
extern void setLineStyle(int style);

extern OverView *ov;

extern QPushButton *pageLabel;

extern QWidget *penSettings;
extern QWidget *colorDialog;
extern QWidget *typeDialog;
extern QSlider *thicknessSlider;
extern QLabel *thicknessLabel;
extern QLabel *colorLabel;

extern QWidget *penTypeDialog;
extern QWidget *modeDialog;


extern "C" {
    extern void *load_archive(void* arg);
    extern QString archive_target;
}

extern void setupWidgets();

extern bool hasSelection;

bool saveImageToFile(const QImage &image, const QString &imageFilePath);
QImage loadImageFromFile(const QString &imageFilePath);

extern void setupBackground();
extern void setupScreenShot();
extern void setupPenType();
extern void setupSaveLoad();

extern void setupWidgets();
extern char* get_icon_by_id(int id);
extern QString get_overlay_by_id(int id);
extern int get_id_by_overlay(QString foverlay);

void setHideMainWindow(bool status);
#ifdef QPRINTER
extern bool PDFMODE;
QImage getPdfImage(int num, float ratio);
void loadPdf(QString path);
void loadPdfFromData(QByteArray data);
#endif
void openFile(QString filename);
