#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"
#include "Button.h"

QPushButton *colorpicker;
DrawingWidget *window;
FloatingWidget *floatingWidget;
WhiteBoard *board;
QMainWindow* mainWindow;
QPushButton *eraser;
QPushButton *pen;
QPushButton *pagemode;
QPushButton *minify;
QString style;


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    mainWindow = new QMainWindow();
    
    board = new WhiteBoard(mainWindow);

    window = new DrawingWidget();
    window->penWidth = 3;
    window->eraserWidth = 100;
    window->setEraser(false);
    window->penColor = QColor("blue");

    mainWindow->setCentralWidget(window);

    floatingWidget = new FloatingWidget(mainWindow);
    floatingWidget->show();
    //floatingWidget->setFixedSize(100,100);

    eraser = create_button("process-stop", [=](){
        window->setEraser(true);
    });
    floatingWidget->setWidget(eraser);

    pen = create_button("process-working-symbolic", [=](){
        window->setEraser(false);
    });
    floatingWidget->setWidget(pen);
    
    pagemode = create_button("process-stop-symbolic", [=](){
        static bool pagemode = true;
        if (pagemode) {
            board->disable();
        } else {
            board->enable();
        }
        pagemode = !pagemode;
    });
    floatingWidget->setWidget(pagemode);

    minify = create_button("printer-symbolic", [=](){
        mainWindow->showMinimized();
    });
    floatingWidget->setWidget(minify);

    colorpicker = create_button("", [=](){
        window->penColor = QColorDialog::getColor(window->penColor, mainWindow, "Select Color");
        style = QString("background-color: %1").arg(window->penColor.name());
        colorpicker->setStyleSheet(style);
    });

    style = QString("background-color: %1").arg(window->penColor.name());
    colorpicker->setStyleSheet(style);
    colorpicker->setFlat(false);

    floatingWidget->setWidget(colorpicker);

    mainWindow->setAttribute(Qt::WA_StaticContents);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow->setAttribute(Qt::WA_NoSystemBackground);
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow->setStyleSheet("background: none");
    mainWindow->showFullScreen();

    return app.exec();
}
