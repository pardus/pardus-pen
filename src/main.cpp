#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;
    FloatingWidget *floatingWidget;
    WhiteBoard *board;
    
    board = new WhiteBoard(&mainWindow);

    DrawingWidget *window = new DrawingWidget();
    window->penWidth = 3;
    window->penColor = QColor("blue");

    mainWindow.setCentralWidget(window);


    // TODO: this is shitty way. replace with real function
    #include "tools/pagemode.h"

    floatingWidget = new FloatingWidget(&mainWindow);
    floatingWidget->show();
    floatingWidget->setWidget(&button);
    floatingWidget->setFixedSize(100,100);

    mainWindow.setAttribute(Qt::WA_StaticContents);
    mainWindow.setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow.setAttribute(Qt::WA_NoSystemBackground);
    mainWindow.setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow.setStyleSheet("background: none");
    mainWindow.showFullScreen();

    return app.exec();
}
