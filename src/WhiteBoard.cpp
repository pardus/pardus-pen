#include <QtWidgets>
#include <QPainter>
#include "WhiteBoard.h"

extern "C" {
#include "settings.h"
}


extern int screenWidth;
extern int screenHeight;

WhiteBoard::WhiteBoard(QWidget *parent) : QWidget(parent) {
    setFixedSize(screenWidth, screenHeight);
    setStyleSheet("background: none");
    show();
}

int WhiteBoard::getType(){
    return type;
}

int WhiteBoard::getOverlayType(){
    return overlayType;
}

void WhiteBoard::setOverlayType(int page){
    set_int((char*)"page-overlay",page);
    overlayType = page;
    update();
}
void WhiteBoard::setType(int page){
    set_int((char*)"page",page);
    type = page;
    if(page == TRANSPARENT){
        background = Qt::transparent;
        lineColor = QColor("#808080");
    } else if (page == BLACK) {
        background = Qt::black;
        lineColor = Qt::white;
    } else {
        background = Qt::white;
        lineColor = Qt::black;
    }
    lineColor.setAlpha(127);
    update();
}

void WhiteBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    gridSize = (float)screenHeight / (float)get_int((char*)"grid-count");
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), background);


    painter.setPen(
        QPen(lineColor, (screenHeight)/1080, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin)
    );

    // Draw the square paper background
    switch(overlayType){
        case NONE:
            break;
        case SQUARES:
            drawSquarePaper();
            break;
        case LINES:
            drawLinePaper();
            break;
        case ISOMETRIC:
            painter.setPen(
                QPen(lineColor, (screenHeight)/(540), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
            );
            drawIsometricPaper();
            break;
    }
    painter.end();
}

void WhiteBoard::drawSquarePaper() {
    // Draw horizontal lines
    for (float y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }

    // Draw vertical lines
    for (float x = 0; x < width(); x += gridSize) {
        painter.drawLine(x, 0, x, height());
    }

}


void WhiteBoard::drawLinePaper() {
    // Draw horizontal lines
    for (float y = 0; y < height(); y += gridSize) {
        painter.drawLine(0, y, width(), y);
    }
}

void WhiteBoard::drawIsometricPaper() {

    for (int y = 0; y <= height(); y += gridSize) {
        for(int x = 0; x <= width(); x += gridSize) {
            if (x + gridSize <= width()) {
                painter.drawLine(x + gridSize,y,x + gridSize,y+1);
            }
            if (y + gridSize <= height()) {
                painter.drawLine(x,y + gridSize,x,y + gridSize+1);
            }
            if (x + 1 <= width() && y + 1 <= height()) {
                painter.drawLine(x+(gridSize/2),y+(gridSize/2),x+(gridSize/2),y+(gridSize/2)+1);
            }
        }
    }

}
