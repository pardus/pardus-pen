#include "DrawingWidget.h"

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    QList<QScreen*> screens = QGuiApplication::screens();
    int screenWidth = 0;
    int screenHeight = 0;
    for (QScreen *screen : screens) {
        screenWidth  += screen->geometry().width();
        screenHeight += screen->geometry().height();
    }
    setFixedSize(screenWidth, screenHeight);
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        drawing = true;
    }
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && drawing) {
        drawLineTo(event->pos());
    }
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
        drawLineTo(event->pos());
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

void DrawingWidget::drawLineTo(const QPoint &endPoint) {
    QPainter painter(&image);
    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawLine(lastPoint, endPoint);
    lastPoint = endPoint;
    update();
}

