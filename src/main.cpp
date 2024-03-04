#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>

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
QPushButton *increase;
QPushButton *decrease;
QString style;

int pagestatus;

int eraser_status;

int main(int argc, char *argv[]) {

    settings_init();

    QApplication app(argc, argv);

    mainWindow = new QMainWindow();

    board = new WhiteBoard(mainWindow);

    window = new DrawingWidget();
    window->penWidth = get_int((char*)"pen-size");
    window->eraserWidth = get_int((char*)"eraser-size");
    window->setEraser(false);
    window->penColor = QColor(get_string((char*)"color"));

    mainWindow->setCentralWidget(window);

    floatingWidget = new FloatingWidget(mainWindow);
    floatingWidget->show();
    //floatingWidget->setFixedSize(100,100);

    /*
      0: eraser
      1: pen
      2: marker
    */
    eraser_status = 1;
    eraser = create_button("process-stop", [=](){
       if (eraser_status == 2) {
           set_icon("printer", eraser);
           eraser_status = 0;
       } else if (eraser_status == 1) {
          set_icon("printer-symbolic", eraser);
          eraser_status = 2;
          window->penColor.setAlpha(128);
       } else{
          set_icon("process-stop", eraser);
          eraser_status = 1;
          window->penColor.setAlpha(255);
       }
       window->setEraser(eraser_status == 0);
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

    minify = create_button("printer-symbolic", [=](){
        mainWindow->showMinimized();
    });
    floatingWidget->setWidget(minify);

    increase = create_button("printer-symbolic", [=](){
        if(window->penWidth < 31) {
            window->penWidth++;
            window->eraserWidth+= 10;
             set_int((char*)"pen-size",window->penWidth);
             set_int((char*)"eraser-sze",window->eraserWidth);
        }
        colorpicker->setText(QString::number(window->penWidth));

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
    colorpicker->setText(QString::number(window->penWidth));
    floatingWidget->setWidget(colorpicker);


    decrease = create_button("printer-symbolic", [=](){
         if(window->penWidth > 1) {
             window->penWidth--;
             window->eraserWidth-= 10;
             set_int((char*)"pen-size",window->penWidth);
             set_int((char*)"eraser-sze",window->eraserWidth);
         }
         colorpicker->setText(QString::number(window->penWidth));
    });
    floatingWidget->setWidget(decrease);


    mainWindow->setAttribute(Qt::WA_StaticContents);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow->setAttribute(Qt::WA_NoSystemBackground);
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow->setStyleSheet("background: none");
    mainWindow->showFullScreen();

    return app.exec();
}
