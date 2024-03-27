#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QColor>

#include <math.h>

#include "OverView.h"


void OverView::updateImage(){
    update();
}

void OverView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    int padding = 8;
    QPen pen(color, penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(pen);
    int w = width() - 2*padding;
    int h = height() - 2*padding - penSize;


    painter.fillRect(rect(), background);

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
