#include "WhiteBoard.h"
extern int screenWidth;
extern int screenHeight;

WhiteBoard::WhiteBoard(QWidget *parent) : QWidget(parent) {
    setFixedSize(screenWidth, screenHeight);
    setStyleSheet("background: white");
    show();
}

void WhiteBoard::enable() {
    setStyleSheet("background: white");
}

void WhiteBoard::enableDark() {
    setStyleSheet("background: black");
}

void WhiteBoard::disable() {
    setStyleSheet("background: none");
}
