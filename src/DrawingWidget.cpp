#include "DrawingWidget.h"
#include "WhiteBoard.h"
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
            // -1 -1 means disable drawing
            return QPointF(-1,-1);
        }
    }

private:
    QMap<qint64, QPointF> values;
};

class ImageStorage {
public:
    int last_image_num = -1;
    int image_count = -1;
    int pageType = TRANSPARENT;
    void saveValue(qint64 id, QImage data) {
        values[id] = data;
    }

    void clear(){
        values.clear();
    }

    QImage loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            return QImage(0,0, QImage::Format_ARGB32);
        }
    }

private:
    QMap<qint64, QImage> values;
};


class PageStorage {
public:
    int last_page_num = 0;
    int page_count = 0;
    void saveValue(qint64 id, ImageStorage data) {
        values[id] = data;
    }

    void clear(){
        values.clear();
    }

    ImageStorage loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            return ImageStorage();
        }
    }

private:
    QMap<qint64, ImageStorage> values;
};

ImageStorage images;

ValueStorage storage;

PageStorage pages;

extern WhiteBoard *board;


int screenWidth = 0;
int screenHeight = 0;

int curEventButtons = 0;
bool isMoved = 0;

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    penType = 1;
    QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        screenWidth  += screen->geometry().width();
        screenHeight += screen->geometry().height();
        break;
    }
    setFixedSize(screenWidth, screenHeight);
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
    drawing = true;
    lastPoint = event->pos();
    curEventButtons = event->buttons();
    isMoved = false;
    if(floatingSettings->isVisible()){
        floatingSettings->hide();
    }

}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    int penTypeBak = penType;
    if(event->buttons() & Qt::RightButton) {
        penType = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        penType = MARKER;
    }
    if (drawing) {
        drawLineTo(event->pos());
    }
    isMoved = true;
    penType = penTypeBak;
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(curEventButtons & Qt::LeftButton && !isMoved) {
        drawLineTo(event->pos()+QPoint(0,1));
    }
    if (drawing) {
       drawing = false;
    }
    images.last_image_num++;
    images.image_count = images.last_image_num;
    images.saveValue(images.last_image_num, image.copy());
    curEventButtons = 0;
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
    images.clear();
    update();
}


int rad = 0;

void DrawingWidget::drawLineTo(const QPoint &endPoint) {
    drawLineToFunc(lastPoint, endPoint, 1.0);
    lastPoint = endPoint;
}

void DrawingWidget::drawLineToFunc(const QPoint startPoint, const QPoint endPoint, qreal pressuse) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    painter.begin(&image);
    switch(penType){
        case ERASER:
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            penColor.setAlpha(255);
            break;
        case MARKER:
            penColor.setAlpha(127);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            break;
        case PEN:
            penColor.setAlpha(255);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            break;
    }
    painter.setPen(QPen(penColor, (penSize[penType]*pressuse*screenHeight)/1080, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    rad = (penSize[penType]*pressuse*screenHeight)/1080;
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawLine(startPoint, endPoint);
    update(QRect(startPoint, endPoint).normalized()
           .adjusted(-rad, -rad, +rad, +rad));
    painter.end();
}

void DrawingWidget::loadImage(int num){
    QImage img = images.loadValue(num);
    QPainter p(&image);
    QRectF target(0, 0, screenWidth, screenHeight);
    QRectF source(0.0, 0.0, screenWidth, screenHeight);
    image.fill(QColor("transparent"));
    p.drawImage(QPoint(0,0), img);
    update();
}

void DrawingWidget::goNextPage(){
    pages.saveValue(pages.last_page_num, images);
    pages.last_page_num++;
    images = pages.loadValue(pages.last_page_num);
    board->setType(images.pageType);
    loadImage(images.last_image_num);
}

void DrawingWidget::goPreviousPage(){
    pages.saveValue(pages.last_page_num, images);
    pages.last_page_num--;
    images = pages.loadValue(pages.last_page_num);
    board->setType(images.pageType);
    loadImage(images.last_image_num);
}

void DrawingWidget::goPrevious(){
    images.last_image_num--;
    loadImage(images.last_image_num);
}


void DrawingWidget::goNext(){
    if(images.last_image_num >= images.image_count){
        return;
    }
    images.last_image_num++;
    loadImage(images.last_image_num);
}

bool DrawingWidget::event(QEvent *ev) {
    switch (ev->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchEnd:
        case QEvent::TouchUpdate: {
            if(floatingSettings->isVisible()){
                floatingSettings->hide();
            }
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
            foreach(const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
            QPointF pos = touchPoint.pos();
            if (touchPoint.state() == Qt::TouchPointPressed) {
                storage.saveValue(touchPoint.id(), pos);
            }
            else if (touchPoint.state() == Qt::TouchPointReleased) {
                storage.saveValue(touchPoint.id(), QPointF(-1,-1));
                continue;
            }
            QPointF oldPos = storage.loadValue(touchPoint.id());
            drawLineToFunc(oldPos.toPoint(), pos.toPoint(), touchPoint.pressure());
            storage.saveValue(touchPoint.id(), pos);
        }
            break;
        }
        default:
            break;
    }
    return QWidget::event(ev);
}

void DrawingWidget::syncPageType(int type){
    images.pageType = type;
}

int DrawingWidget::getPageNum(){
    return pages.last_page_num;
}


QColor convertColor(QColor color) {
    int tot =  color.red() + color.blue() + color.green();
    if (tot > 382) {
        return QColor(0,0,0, color.alpha());
    } else {
        return QColor(255, 255, 255, color.alpha());

    }
}
