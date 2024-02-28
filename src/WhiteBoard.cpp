#include "WhiteBoard.h"

WhiteBoard::WhiteBoard(QWidget *parent) : QWidget(parent) {
    QList<QScreen*> screens = QGuiApplication::screens();
    int screenWidth = 0;
    int screenHeight = 0;
    for (QScreen *screen : screens) {
        screenWidth  += screen->geometry().width();
        screenHeight += screen->geometry().height();
    }
    setFixedSize(screenWidth, screenHeight);
    show();
}

void WhiteBoard::enable() {
    setStyleSheet("background: white");
}

void WhiteBoard::disable() {
    setStyleSheet("background: none");
}
