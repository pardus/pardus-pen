#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QDebug>
#include <QRect>

#include "ScreenshotWidget.h"
#include "../tools.h"


ScreenshotWidget::ScreenshotWidget(QWidget *parent)
    : QWidget(parent), selecting(false) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
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
    painter.setPen(Qt::NoPen);
    // background
    painter.setBrush(QBrush(QColor(255, 255, 255, 13)));
    painter.drawRect(QRect(0,0,geometry().width(), geometry().height()));
    // selection
    QColor color = drawing->penColor;
    color.setAlpha(127);
    painter.setBrush(QBrush(color));
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

