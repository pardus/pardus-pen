#include <QApplication>
#include <QWidget>
#include <QMainWindow>

#include "DrawingWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow mainWindow;

    DrawingWidget *window = new DrawingWidget();
    window->penWidth = 3;
    window->penColor = QColor("blue");

    mainWindow.setCentralWidget(window);
    mainWindow.setAttribute(Qt::WA_StaticContents);
    mainWindow.setAttribute(Qt::WA_TranslucentBackground, true);
    mainWindow.setAttribute(Qt::WA_NoSystemBackground);

    mainWindow.setStyleSheet("background: none");
    mainWindow.showFullScreen();

    return app.exec();
}
