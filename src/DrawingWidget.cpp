#include "DrawingWidget.h"
#include <stdio.h>

/*
penType:
 - 0 eraser
 - 1 pen
 - 2 marker
*/

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    penType = 1;
    QList<QScreen*> screens = QGuiApplication::screens();
    int screenWidth = 0;
    int screenHeight = 0;
    for (QScreen *screen : screens) {
        screenWidth  += screen->geometry().width();
        screenHeight += screen->geometry().height();
        break;
    }
    setFixedSize(screenWidth, screenHeight);
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drawing = true;
        lastPoint = event->pos();
    }
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && drawing) {
        drawLineTo(event->pos());
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
       drawLineTo(event->pos()+QPoint(0,1));
       drawing = false;
    }
}

void DrawingWidget::initializeImage(const QSize &size) {
    image = QImage(size, QImage::Format_ARGB32);
    image.fill(QColor("transparent"));
}

void DrawingWidget::resizeEvent(QResizeEvent *event) {
    initializeImage(event->size());
    QWidget::resizeEvent(event);
}

void DrawingWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawImage(0, 0, image);
}


void DrawingWidget::clear() {
    image.fill(QColor("transparent"));
    update();
}


int rad = 0;

void DrawingWidget::drawLineTo(const QPoint &endPoint) {
    QPainter painter(&image);
    switch(penType){
        case ERASER:
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            break;
        case PEN:
        case MARKER:
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            break;
    }
    painter.setPen(QPen(penColor, penSize[penType], Qt::SolidLine, Qt::RoundCap));
    rad = penSize[penType];
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(lastPoint, endPoint);
    update(QRect(lastPoint, endPoint).normalized()
           .adjusted(-rad, -rad, +rad, +rad));

    lastPoint = endPoint;
}

bool DrawingWidget::event(QEvent *ev) {
  switch (ev->type()) {
    case QEvent::TouchBegin:
        std::cout << "Event began." << std::endl;
        break;
    case QEvent::TouchEnd:
        std::cout << "Event end." << std::endl;
        break;
    case QEvent::TouchUpdate:
        std::cout << "Event update." << std::endl;
        break;
    default:
        //std::cout << "Misc event." << std::endl;
        break;
    }
    return QWidget::event(ev);
}


QColor convertColor(QColor color) {
    int tot =  color.red() + color.blue() + color.green();
    if (tot > 382) {
        return QColor(0,0,0, color.alpha());
    } else {
        return QColor(255, 255, 255, color.alpha());

    }
}
