#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>

#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"
#include "Button.h"

#define _(String) gettext(String)

extern "C" {
#include "settings.h"
}

QPushButton *colorpicker;
DrawingWidget *window;
FloatingWidget *floatingWidget;
WhiteBoard *board;
QMainWindow* mainWindow;
QPushButton *eraser;
QPushButton *pagemode;
QPushButton *minify;
QPushButton *clear;
QPushButton *increase;
QPushButton *decrease;
QPushButton *close;
QString style;

#ifdef screenshot
#include "ScreenShot.h"
QPushButton *ssButton;
#endif


int pagestatus;

int eraser_status;

int main(int argc, char *argv[]) {

    // Force use X11 or Xwayland
    setenv("QT_QPA_PLATFORM", "xcb",1);

    settings_init();

    // translation part
    const char *systemLanguage = std::getenv("LANG");
    if (systemLanguage != nullptr) {
        bindtextdomain("pardus-pen", "/usr/share/locale");
        setlocale(LC_ALL, systemLanguage);
        textdomain("pardus-pen");
    } else {
        std::cerr << "LANG environment variable not set." << std::endl;
        return 1;
    }

    QApplication app(argc, argv);

    mainWindow = new QMainWindow();
    window = new DrawingWidget();
    board = new WhiteBoard(mainWindow);

    window->penSize[PEN] = get_int((char*)"pen-size");
    window->penSize[ERASER] = get_int((char*)"eraser-size");
    window->penSize[MARKER] = get_int((char*)"marker-size");
    window->penType=PEN;
    window->penColor = QColor(get_string((char*)"color"));

    mainWindow->setCentralWidget(window);

    floatingWidget = new FloatingWidget(mainWindow);
    floatingWidget->show();

    eraser_status = 1;
    eraser = create_button(":images/pen.svg", [=](){
        switch(eraser_status) {
            case MARKER:
                set_icon(":images/eraser.svg", eraser);
                eraser_status = 0;
                window->penType = ERASER;
                break;
            case PEN:
                set_icon(":images/marker.svg", eraser);
                eraser_status = 2;
                window->penColor.setAlpha(128);
                window->penType = MARKER;
                break;
            case ERASER:
                set_icon(":images/pen.svg", eraser);
                eraser_status = 1;
                window->penColor.setAlpha(255);
                window->penType = PEN;
                break;
       }
      colorpicker->setText(QString::number(window->penSize[window->penType]));
   });
    floatingWidget->setWidget(eraser);

    pagemode = create_button(":images/paper-transparent.svg", [=](){
        if (pagestatus == 2) {
            board->disable();
            set_icon(":images/paper-transparent.svg", pagemode);
            pagestatus = 0;
        } else if (pagestatus == 1) {
            board->enableDark();
            pagestatus = 2;
            set_icon(":images/paper-black.svg", pagemode);
        } else {
            board->enable();
            pagestatus = 1;
            set_icon(":images/paper-white.svg", pagemode);
        }
    });
    board->disable();
    pagestatus = 0;
    floatingWidget->setWidget(pagemode);

    minify = create_button(":images/screen.svg", [=](){
        mainWindow->showMinimized();
    });
    floatingWidget->setWidget(minify);

    clear = create_button(":images/clear.svg", [=](){
        window->clear();
    });
    floatingWidget->setWidget(clear);

    increase = create_button(":images/increase.svg", [=](){
        window->penSize[window->penType]++;
        set_int((char*)"pen-size",window->penSize[PEN]);
        set_int((char*)"eraser-size",window->penSize[ERASER]);
        set_int((char*)"marker-size",window->penSize[MARKER]);
        colorpicker->setText(QString::number(window->penSize[window->penType]));

    });
    floatingWidget->setWidget(increase);


    colorpicker = create_button("", [=](){
        window->penColor = QColorDialog::getColor(window->penColor, mainWindow, "Select Color");
        set_string((char*)"color", (char*)window->penColor.name().toStdString().c_str());
        style = QString(
            "color: " + convertColor(window->penColor).name() + "; "
            "border-radius:0px;"
            "background-color: " + window->penColor.name());
        colorpicker->setStyleSheet(style);
        if (eraser_status == 2) {
            window->penColor.setAlpha(128);
        } else if (eraser_status == 1) {
            window->penColor.setAlpha(255);
        }
    });
    style = QString(
        "color: " + convertColor(window->penColor).name() + "; "
        "border-radius:0px;"
        "background-color: " + window->penColor.name());
    colorpicker->setStyleSheet(style);
    colorpicker->setFlat(false);
    colorpicker->setText(QString::number(window->penSize[window->penType]));
    floatingWidget->setWidget(colorpicker);


    decrease = create_button(":images/decrease.svg", [=](){
        if(window->penSize[window->penType] > 1) {
            window->penSize[window->penType]--;
            set_int((char*)"pen-size",window->penSize[PEN]);
            set_int((char*)"eraser-size",window->penSize[ERASER]);
            set_int((char*)"marker-size",window->penSize[MARKER]);
         }
         colorpicker->setText(QString::number(window->penSize[window->penType]));
    });
    floatingWidget->setWidget(decrease);

    #ifdef screenshot

    ssButton = create_button(":images/screenshot.svg", [=](){
        takeScreenshot();
    });
    floatingWidget->setWidget(ssButton);

    #endif

    close = create_button(":images/close.svg", [=](){
        QMessageBox msgBox;
        msgBox.setWindowFlags(Qt::Dialog | Qt::X11BypassWindowManagerHint);
        msgBox.setWindowTitle(_("Quit"));
        msgBox.setText(_("Are you want to quit pardus pen?"));
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            QApplication::quit();
        }
    });
    floatingWidget->setWidget(close);


    mainWindow->setAttribute(Qt::WA_StaticContents);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow->setAttribute(Qt::WA_NoSystemBackground);
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow->setStyleSheet("background: none");
    mainWindow->showFullScreen();

    return app.exec();
}
