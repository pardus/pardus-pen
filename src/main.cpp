#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>

#include <stdlib.h>


#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"
#include "Button.h"

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
QString style;

#ifdef ffmpeg
QPushButton *record;
#endif


int pagestatus;

int eraser_status;

int main(int argc, char *argv[]) {

    // Force use X11 or Xwayland
    setenv("QT_QPA_PLATFORM", "xcb",1);

    settings_init();

    QApplication app(argc, argv);

    mainWindow = new QMainWindow();

    board = new WhiteBoard(mainWindow);

    window = new DrawingWidget();
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

    pagemode = create_button("process-stop-symbolic", [=](){
        if (pagestatus == 2) {
            board->disable();
            set_icon("gtk-yes", pagemode);
            pagestatus = 0;
        } else if (pagestatus == 1) {
            board->enableDark();
            pagestatus = 2;
            set_icon("gtk-no", pagemode);
        } else {
            board->enable();
            pagestatus = 1;
            set_icon("process-stop-symbolic", pagemode);
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

    increase = create_button("printer-symbolic", [=](){
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


    decrease = create_button("printer-symbolic", [=](){
        if(window->penSize[window->penType] > 1) {
            window->penSize[window->penType]--;
            set_int((char*)"pen-size",window->penSize[PEN]);
            set_int((char*)"eraser-size",window->penSize[ERASER]);
            set_int((char*)"marker-size",window->penSize[MARKER]);
         }
         colorpicker->setText(QString::number(window->penSize[window->penType]));
    });
    floatingWidget->setWidget(decrease);
    #ifdef ffmpeg

    record = create_button(":images/record.svg", [=](){
        QString pics = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        QDateTime time = QDateTime::currentDateTime();
        QString imgname = pics + "/" + time.toString("yyyy-MM-dd_hh-mm-ss") + ".png";
        char *cmd = (char*)malloc(1024*sizeof(char));
        strcpy(cmd,"scrot '");
        strcat(cmd,imgname.toStdString().c_str());
        strcat(cmd,"'");
        system(cmd);

    });
    floatingWidget->setWidget(record);

    #endif



    mainWindow->setAttribute(Qt::WA_StaticContents);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow->setAttribute(Qt::WA_NoSystemBackground);
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow->setStyleSheet("background: none");
    mainWindow->showFullScreen();

    return app.exec();
}
