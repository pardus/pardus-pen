#include <QtWidgets>
#include <QPainter>
#include "WhiteBoard.h"


#include "../tools.h"

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
    if(page != CUSTOM) {
        overlays[drawing->getPageNum()].fill(QColor("transparent"));;
    }
    set_int("page-overlay",page);
    overlayType = page;
    update();
}
void WhiteBoard::setType(int page){
    set_int("page",page);
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
    overlays[drawing->getPageNum()] = image;
    updateTransform();
    update();
}

void WhiteBoard::updateTransform(){
    int w = mainWindow->geometry().width();
    int h = mainWindow->geometry().height();
    if (!ratios.contains(drawing->getPageNum())
        || ratios[drawing->getPageNum()] <= 0){
        ratios[drawing->getPageNum()] = 100.0;
    }
    float ratio = ratios[drawing->getPageNum()] / 100.0;
    QImage img;
    #ifdef QPRINTER
    if(PDFMODE){
       img = getPdfImage(drawing->getPageNum(), ratio);
    } else {
    #endif
        img = overlays[drawing->getPageNum()];
    #ifdef QPRINTER
    }
    #endif
    if(img.isNull()){
        transformImage = QImage();
        return;
    }
    QTransform transform;
    transform.rotate(rotates[drawing->getPageNum()]);
    #ifdef QPRINTER
    if(PDFMODE){
        transformImage = img.transformed(transform);
        return;
    }
    #endif
    if(img.size().width() * img.size().height() > 0){
        w = img.size().width() * h / img.size().height();
        if(w > mainWindow->geometry().width()) {
            w = mainWindow->geometry().width();
            h = img.size().height() * w / img.size().width();
        }
        w = w*ratio;
        h = h*ratio;
    }
    transformImage = img.scaled(w,h).transformed(transform);
}

void WhiteBoard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), background);

    painter.setPen(
        QPen(lineColor, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin)
    );


    if (!ratios.contains(drawing->getPageNum())
        || ratios[drawing->getPageNum()] <= 0){
        ratios[drawing->getPageNum()] = 100.0;
    }
    float ratio = ratios[drawing->getPageNum()] / 100.0;


    #ifdef QPRINTER
    if(PDFMODE){
        overlayType = CUSTOM;
    }
    #endif
    gridSize = (float)mainWindow->geometry().height() / (float)get_int("grid-count") * ratio;
    // Draw the square paper background
    switch(overlayType){
        case BLANK:
            break;
        case CUSTOM:
            ow = (mainWindow->geometry().width() - transformImage.size().width()) / 2;
            oh = (mainWindow->geometry().height() - transformImage.size().height()) / 2;
            painter.drawImage(QPoint(ow, oh),transformImage);
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

