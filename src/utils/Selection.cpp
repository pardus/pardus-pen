#include "../widgets/DrawingWidget.h"
#include "../utils/Selection.h"
#include "../tools.h"

MovableWidget::MovableWidget(QWidget *parent) : QWidget(parent) {
    crop = new QLabel("");
}

extern float scale;
#define nearEdge 10*scale

void MovableWidget::mousePressEvent(QMouseEvent *event) {
    lastMousePosition = event->pos();
    int xx = width() - event->pos().x();
    int yy = height() - event->pos().y();
    if(width() - xx < 50 || xx < 50 || height() - yy < 50 || yy < 50) {
        mode = RESIZE;
    } else{
        mode = DRAG;
    }
}

void MovableWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint newPos;
    if(mode == RESIZE) {
        QPoint center = QPoint(pos().x() + (width()/2), pos().y() + (height()/2));
        setFixedSize(
            abs((mapToParent(event->pos()).x() - center.x())*2),
            abs((mapToParent(event->pos()).y() - center.y())*2)
        );
        QPixmap pixmap = QPixmap::fromImage(image.scaled(width()*mainWidget->devicePixelRatio(), height()*mainWidget->devicePixelRatio()));
        pixmap.setDevicePixelRatio(mainWidget->devicePixelRatio());
        crop->setPixmap(pixmap);
        newPos = QPoint(center.x() - (width()/2), center.y() - (height()/2));
    } else if (mode == DRAG) {
        newPos = mapToParent(event->pos() - lastMousePosition);
    }
    move(newPos);
}

void MovableWidget::mouseReleaseEvent(QMouseEvent *event) {
    (void)event;
}

bool hasSelection = false;

void DrawingWidget::createSelection(int source) {
    debug("source: %d\n", source);
    hasSelection = true;
    QPointF startPoint = geo.first(source);
    QPointF endPoint = geo.last(source);
    debug("x1: %f x2: %f y1: %f y2: %f\n", startPoint.x(), endPoint.x(), startPoint.y(), endPoint.y());
    QPoint topLeft(qMin(startPoint.x(), endPoint.x()), qMin(startPoint.y(), endPoint.y()));
    QPoint bottomRight(qMax(startPoint.x(), endPoint.x()), qMax(startPoint.y(), endPoint.y()));
    QRect cropRect(topLeft, bottomRight);


    image.fill(QColor("transparent"));

    QPixmap pix = QPixmap(cropRect.size()*mainWidget->devicePixelRatio());
    pix.setDevicePixelRatio(mainWidget->devicePixelRatio());
    pix.fill(QColor("transparent"));
    cropWidget->image = pix.toImage();
    painter.begin(&(cropWidget->image));
    painter.setPen(Qt::NoPen);
    painter.drawRect(cropRect);
    painter.drawImage(
        topLeft.x()*-1,
        topLeft.y()*-1,
        background->image);
    painter.end();

    painter.begin(&(background->image));
    painter.setBrush(QBrush(penColor));
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setPen(Qt::NoPen);
    painter.drawRect(cropRect);
    painter.end();

    debug("width: %d height: %d\n", cropRect.width(), cropRect.height());
    cropWidget->setFixedSize(cropRect.width(), cropRect.height());
    QPixmap pixmap = QPixmap::fromImage(cropWidget->image);
    cropWidget->crop->setPixmap(pixmap);
    cropWidget->move(topLeft);
    cropWidget->raise();
    cropWidget->show();
    update(cropRect);
}

void DrawingWidget::clearSelection() {
    if(!hasSelection){
        return;
    }
    hasSelection = false;
    cropWidget->setFixedSize(0,0);
    cropWidget->move(QPoint(-1,-1));
    cropWidget->image = QImage(QSize(0,0), QImage::Format_ARGB32);
    cropWidget->image.fill(QColor("transparent"));
    addImage(image.toImage());

}

void DrawingWidget::mergeSelection() {
    if(!hasSelection){
        return;
    }
    painter.begin(&image);
    painter.setPen(Qt::NoPen);
    painter.drawImage(
        QPoint(cropWidget->x(), cropWidget->y()),
         cropWidget->image.scaled(
             cropWidget->width()*mainWidget->devicePixelRatio(),
             cropWidget->height()*mainWidget->devicePixelRatio()
         )
    );
    update(
        cropWidget->x(),
        cropWidget->y(),
        cropWidget->x()+cropWidget->width(),
        cropWidget->y()+cropWidget->height()
    );
    painter.end();
    clearSelection();
}
