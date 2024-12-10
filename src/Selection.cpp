#include "DrawingWidget.h"
#include "Selection.h"

MovableWidget::MovableWidget(QWidget *parent) : QWidget(parent) {}

void MovableWidget::mousePressEvent(QMouseEvent *event) {
     (void)event;
     lastMousePosition = event->pos();
     dragging = true;
}

void MovableWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint newPos = mapToParent(event->pos() - lastMousePosition);
    move(newPos); // Move the widget to the new position
}

void MovableWidget::mouseReleaseEvent(QMouseEvent *event) {
    (void)event;
    dragging = false;
}

static bool hasSelection = false;

void DrawingWidget::createSelection() {
    hasSelection = true;
    QPoint topLeft(qMin(firstPoint.x(), lastPoint.x()), qMin(firstPoint.y(), lastPoint.y()));
    QPoint bottomRight(qMax(firstPoint.x(), lastPoint.x()), qMax(firstPoint.y(), lastPoint.y()));
    QRect cropRect(topLeft, bottomRight);

    cropped = imageBackup.copy(cropRect);

    painter.begin(&image);
    painter.setBrush(QBrush(penColor));
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.setPen(Qt::NoPen);
    painter.drawRect(cropRect);
    update();
    painter.end();

    printf("%d %d\n", cropRect.width(), cropRect.height());
    cropWidget->setFixedSize(cropRect.width(), cropRect.height());
    QPixmap pixmap = QPixmap::fromImage(cropped);
    crop->setPixmap(pixmap);
    cropWidget->move(topLeft);
    cropWidget->raise();
    cropWidget->show();
    update();
}

void DrawingWidget::mergeSelection() {
    if(!hasSelection){
        return;
    }
    hasSelection = false;
    painter.begin(&image);
    painter.setPen(Qt::NoPen);
    painter.drawImage(QPoint(cropWidget->x(), cropWidget->y()), cropped);
    cropWidget->setFixedSize(0,0);
    cropWidget->move(QPoint(-1,-1));
    update();
    painter.end();
    addImage(image);
    
}
