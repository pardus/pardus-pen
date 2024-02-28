#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "WhiteBoard.h"
#include "Button.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow* mainWindow = new QMainWindow();
    FloatingWidget *floatingWidget;
    WhiteBoard *board;
    
    board = new WhiteBoard(mainWindow);

    DrawingWidget *window = new DrawingWidget();
    window->penWidth = 3;
    window->eraserWidth = 100;
    window->setEraser(false);
    window->penColor = QColor("blue");

    mainWindow->setCentralWidget(window);

    floatingWidget = new FloatingWidget(mainWindow);
    floatingWidget->show();
    //floatingWidget->setFixedSize(100,100);

    QPushButton *eraser = create_button("process-stop", [=](){
        window->setEraser(true);
    });
    floatingWidget->setWidget(eraser);

    QPushButton *pen = create_button("process-working-symbolic", [=](){
        window->setEraser(false);
    });
    floatingWidget->setWidget(pen);
    
    QPushButton *pagemode = create_button("process-stop-symbolic", [=](){
        static bool pagemode = true;
        if (pagemode) {
            board->disable();
        } else {
            board->enable();
        }
        pagemode = !pagemode;
    });
    floatingWidget->setWidget(pagemode);

    QPushButton *minify = create_button("printer-symbolic", [=](){
        mainWindow->showMinimized();
    });
    floatingWidget->setWidget(minify);

    QPushButton *colorpicker = create_button("printer", [=](){
        QColor currentColor;
        window->penColor = QColorDialog::getColor(window->penColor, mainWindow, "Select Color");
    });
    floatingWidget->setWidget(colorpicker);

    mainWindow->setAttribute(Qt::WA_StaticContents);
    mainWindow->setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow->setAttribute(Qt::WA_NoSystemBackground);
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
    mainWindow->setStyleSheet("background: none");
    mainWindow->showFullScreen();

    return app.exec();
}
