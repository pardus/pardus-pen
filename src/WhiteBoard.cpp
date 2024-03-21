#include "WhiteBoard.h"

extern "C" {
#include "settings.h"
}

extern int screenWidth;
extern int screenHeight;

WhiteBoard::WhiteBoard(QWidget *parent) : QWidget(parent) {
    setFixedSize(screenWidth, screenHeight);
    setStyleSheet("background: white");
    show();
}

void WhiteBoard::enable() {
    setStyleSheet("background: white");
    type = WHITE;
}

void WhiteBoard::enableDark() {
    setStyleSheet("background: black");
    type = BLACK;
}

void WhiteBoard::disable() {
    setStyleSheet("background: none");
    type = TRANSPARENT;
}

int WhiteBoard::getType(){
    return type;
}

void WhiteBoard::setType(int page){
    set_int((char*)"page",page);
    if(page == TRANSPARENT){
        disable();
    } else if (page == BLACK) {
        enableDark();
    } else {
        enable();
    }

}
