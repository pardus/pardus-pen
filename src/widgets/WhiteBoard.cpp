#include <QtWidgets>
#include <QPainter>
#include "WhiteBoard.h"

extern "C" {
#include "../utils/settings.h"
}


#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#define _(String) gettext(String)

WhiteBoard::WhiteBoard(QWidget *parent) : QWidget(parent) {
    setStyleSheet("background: none");
    mainWindow = (QMainWindow*)parent;
    show();
}

int WhiteBoard::getType(){
    return type;
}

int WhiteBoard::getOverlayType(){
    return overlayType;
}

void WhiteBoard::setOverlayType(int page){
    if(page == CUSTOM) {
        backgroundImage.fill(QColor("transparent"));
    } else if (page == TURKIYE){
        backgroundImage = QImage(":images/turkiye-map.svg");
    } else if (page == WORLD){
        backgroundImage = QImage(":images/world-map.svg");
    } else {
        backgroundImage.fill(QColor("transparent"));
        set_int((char*)"page-overlay",page);
    }
    overlayType = page;
    repaint();
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
    repaint();
}

void WhiteBoard::setImage(QImage image){
    backgroundImage = image;
    repaint();
}

void WhiteBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    drawAction(this);
}

void WhiteBoard::drawAction(QWidget* widget) {

    gridSize = (float)mainWindow->geometry().height() / (float)get_int((char*)"grid-count");
    painter.begin(widget);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), background);

    painter.setPen(
        QPen(lineColor, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin)
    );

    int w = mainWindow->geometry().width();
    int h = mainWindow->geometry().height();
    // Draw the square paper background
    switch(overlayType){
        case NONE:
            break;
        case CUSTOM:
        case TURKIYE:
        case WORLD:
            // %10 padding
            painter.drawImage(QPoint(w*0.1, h*0.1), backgroundImage.scaled(w*0.8, h*0.8));
            break;
        case SQUARES:
            drawSquarePaper();
            break;
        case LINES:
            drawLinePaper();
            break;
        case ISOMETRIC:
            painter.setPen(
                QPen(lineColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
            );
            drawIsometricPaper();
            break;
        case MUSIC:
            drawMusicPaper();
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
                painter.drawLine((x + gridSize)*1.44,y,(x + gridSize)*1.44,y+1);
            }
            if (x + 1 <= width() && y + 1 <= height()) {
                painter.drawLine((x+(gridSize/2))*1.44,y+(gridSize/2),(x+(gridSize/2))*1.44,y+(gridSize/2)+1);
            }
        }
    }

}

void WhiteBoard::drawMusicPaper() {
    painter.setPen(
        QPen(lineColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    );

    for (int y = gridSize* 2; y < height() - gridSize; y += gridSize * 5) {
        for (int i = 0; i < 5; ++i) {
            painter.drawLine(gridSize, y + i * gridSize / 2 , width() - gridSize, y + i * gridSize / 2);
        }
    }

}
