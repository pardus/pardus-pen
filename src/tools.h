
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

#include "utils/Settings.h"

extern QMap<qint64, QPushButton*> penButtons;
extern QMap<qint64, QPushButton*> toolButtons;

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
extern int getPen();
extern void setPenStyle(int style);
extern void setLineStyle(int style);

extern OverView *ov;

extern QLabel *pageLabel;

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
extern void penSizeEvent();
extern void penStyleEvent();
extern void setupWidgets();

extern bool hasSelection;

bool saveImageToFile(const QImage &image, const QString &imageFilePath);
QImage loadImageFromFile(const QString &imageFilePath);

extern void setupBackground();
extern void setupScreenShot();
extern void setupPenType();
extern void setupSaveLoad();
extern void backgroundStyleEvent();

extern void setupWidgets();

void setHideMainWindow(bool status);
#ifdef QPRINTER
extern bool PDFMODE;
QImage getPdfImage(int num, float ratio);
void loadPdf(QString path);
#endif
void openFile(QString filename);
