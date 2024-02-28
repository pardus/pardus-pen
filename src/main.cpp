#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"
#include "Button.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    FloatingWidget *floatingWidget;
    WhiteBoard *board;
    
    board = new WhiteBoard(&mainWindow);

    DrawingWidget *window = new DrawingWidget();
    window->penWidth = 3;
    window->eraserWidth = 100;
    window->setEraser(false);
    window->penColor = QColor("blue");

    mainWindow.setCentralWidget(window);

    floatingWidget = new FloatingWidget(&mainWindow);
    floatingWidget->show();
    floatingWidget->setFixedSize(100,100);

    QPushButton *eraser = create_button([=](){
        window->setEraser(true);
    });
    floatingWidget->setWidget(eraser);

    QPushButton *pen = create_button([=](){
        window->setEraser(false);
    });
    floatingWidget->setWidget(pen);

    mainWindow.setAttribute(Qt::WA_StaticContents);
    mainWindow.setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow.setAttribute(Qt::WA_NoSystemBackground);
    mainWindow.setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow.setStyleSheet("background: none");
    mainWindow.showFullScreen();

    return app.exec();
}
