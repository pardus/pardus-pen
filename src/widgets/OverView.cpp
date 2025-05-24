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


OverView::OverView(QWidget *parent) : QWidget(parent) {
    setStyleSheet(
    "background: none;");
}

void OverView::updateImage(){
    update();
}
void OverView::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    int penSize = drawing->penSize[getPen()];
    int penType = getPen();
    QPen pen(QColor("#f3232323"),12*scale,  Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setBrush(QBrush(board->background));
    painter.setPen(pen);
    painter.drawRoundedRect(rect().adjusted(6*scale, 6*scale, -6*scale, -6*scale), 12*scale, 13*scale);
    int w = width() - 2*padding ;
    int h = height() - 2*padding ;
    painter.end();

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    pen.setColor(drawing->penColor);
    pen.setWidth(penSize);
    painter.setPen(pen);


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
        QPainterPath path;
        int xPrev = 0, yPrev = 0;
        for (int x = padding; x <= w - padding ; x+=scale) {
            double y = ((h -2*penSize) / 2) * sin(2 * M_PI * x / w) + h  / 2;
            if (x > padding) {
                path.moveTo(QPointF(xPrev+padding, yPrev+padding));
                path.lineTo(QPointF(x+padding, y+padding+(penSize/2)));
            }
            xPrev = x;
            yPrev = y;
        }
        painter.drawPath(path);
    }
}
