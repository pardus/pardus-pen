#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QSvgRenderer>
#include <QPixmap>

#include <math.h>

#include "../tools.h"

#include "OverView.h"
#include "DrawingWidget.h"


void OverView::updateImage(){
    update();
}

void OverView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    int penSize = drawing->penSize[drawing->penType];
    int penType = drawing->penType;
    QPen pen(drawing->penColor, penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(pen);
    int w = width() - 2*padding;
    int h = height() - 2*padding - penSize;


    painter.fillRect(rect(), board->background);

    if(penType == ERASER){
        QSvgRenderer svgRenderer(QStringLiteral(":/images/cursor.svg"));

        QSize pixmapSize(penSize, penSize);
        QPixmap pixmap(pixmapSize);
        // Render the SVG onto the QPixmap
        pixmap.fill(Qt::transparent);
        QPainter pp(&pixmap);
        svgRenderer.render(&pp);
        pp.end();
        
        int w1 = (geometry().width() - penSize) / 2;
        int h1 = (geometry().height() - penSize) / 2;
        painter.drawPixmap(QRect(w1, h1, penSize, penSize), pixmap);
    } else {
        // Draw the sine wave
        int xPrev, yPrev;
        for (int x = 0; x <= w; x++) {
            double y = (h / 2) * sin(2 * M_PI * x / w) + h / 2;
            if (x > 0) {
                painter.drawLine(xPrev+padding, yPrev+padding+(penSize/2), x+padding, y+padding+(penSize/2));
            }
            xPrev = x;
            yPrev = y;
        }
    }
}
