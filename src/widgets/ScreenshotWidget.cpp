#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QDebug>
#include <QRect>

#include "ScreenshotWidget.h"


ScreenshotWidget::ScreenshotWidget(QWidget *parent)
    : QWidget(parent), selecting(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setCursor(Qt::CrossCursor);
    hide();
}

void ScreenshotWidget::mousePressEvent(QMouseEvent *event) {
    startPos = event->pos();
}

void ScreenshotWidget::mouseMoveEvent(QMouseEvent *event) {
    endPos = event->pos();
    update();
}

void ScreenshotWidget::mouseReleaseEvent(QMouseEvent *event) {
    endPos = event->pos();
    cropScreenshot();
    startPos = QPoint(-1, -1);
    endPos = QPoint(-1, -1);
    update();
}

void ScreenshotWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setBrush(QColor(0, 0, 255, 50)); // Semi-transparent blue
    QRect rect(startPos, endPos);
    painter.drawRect(rect.normalized());
}

void ScreenshotWidget::cropScreenshot() {
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap originalPixmap = screen->grabWindow(0);

    QRect cropRect(startPos, endPos);
    QPixmap croppedPixmap = originalPixmap.copy(cropRect.normalized());
    crop_signal(croppedPixmap);
    hide();
}

