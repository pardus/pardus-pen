#include "DrawingWidget.h"
#include <stdio.h>

/*
penType:
 - 0 eraser
 - 1 pen
 - 2 marker
*/

#include <QDebug>
#include <QMap>

class ValueStorage {
public:
    void saveValue(qint64 id, QPointF data) {
        values[id] = data;
    }

    QPointF loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            printf("%d %s\n", id, "not found in storage");
            return QPointF(-1,-1);
        }
    }

private:
    QMap<qint64, QPointF> values;
};

ValueStorage storage;


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
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(0, 0, image);
    painter.end();
}


void DrawingWidget::clear() {
    image.fill(QColor("transparent"));
    update();
}


int rad = 0;

void DrawingWidget::drawLineTo(const QPoint &endPoint) {
    drawLineToFunc(lastPoint, endPoint);
    lastPoint = endPoint;
}

void DrawingWidget::drawLineToFunc(const QPoint startPoint, const QPoint endPoint) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    painter.begin(&image);
    switch(penType){
        case ERASER:
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            break;
        case PEN:
        case MARKER:
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            break;
    }
    painter.setPen(QPen(penColor, penSize[penType], Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    rad = penSize[penType];
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawLine(startPoint, endPoint);
    update(QRect(startPoint, endPoint).normalized()
           .adjusted(-rad, -rad, +rad, +rad));
    painter.end();
}

bool DrawingWidget::event(QEvent *ev) {
    switch (ev->type()) {
        case QEvent::TouchBegin: {
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            for (const QTouchEvent::TouchPoint &touchPoint : touchPoints) {
                QPointF pos = touchPoint.pos(); // Retrieve touch position
                storage.saveValue(touchPoint.id(), pos);
                printf("%ld begin\n",touchPoint.id());
            }
            break;
        }
        case QEvent::TouchEnd: {
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            for (const QTouchEvent::TouchPoint &touchPoint : touchPoints) {
                QPointF pos = touchPoint.pos(); // Retrieve touch position
                storage.saveValue(touchPoint.id(), QPointF(-1,-1));
                printf("%ld end\n",touchPoint.id());
            }
            break;
        }
        case QEvent::TouchUpdate: {
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            for (const QTouchEvent::TouchPoint &touchPoint : touchPoints) {
                QPointF oldPos = storage.loadValue(touchPoint.id());
                QPointF pos = touchPoint.pos(); // Retrieve touch position
                drawLineToFunc(oldPos.toPoint(), pos.toPoint());
                storage.saveValue(touchPoint.id(), pos);
            }
            break;
        }
        default:
            printf("%d hmm\n", ev->type());
            // Handle other events if needed
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
