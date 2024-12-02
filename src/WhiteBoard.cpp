#include <QtWidgets>
#include <QPainter>
#include "WhiteBoard.h"

extern "C" {
#include "settings.h"
}



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

void WhiteBoard::setImage(QImage image){
    backgroundImage = image;
    update();
}

void WhiteBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    gridSize = (float)mainWindow->geometry().height() / (float)get_int((char*)"grid-count");
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), background);

    if (backgroundImage.width() > 0 && backgroundImage.height() > 0){
        int h1 = (mainWindow->geometry().height() - backgroundImage.height() ) / 2;
        int w1 = (mainWindow->geometry().width() - backgroundImage.width() ) / 2;
        painter.drawImage(QPoint(w1, h1), backgroundImage);
    }

    painter.setPen(
        QPen(lineColor, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin)
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
    QPainter painter(this);
    painter.setPen(
        QPen(lineColor, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
    );

    for (int y = gridSize* 2; y < height() - gridSize; y += gridSize * 5) {
        for (int i = 0; i < 5; ++i) {
            painter.drawLine(gridSize, y + i * gridSize / 2 , width() - gridSize, y + i * gridSize / 2);
        }
    }

}
