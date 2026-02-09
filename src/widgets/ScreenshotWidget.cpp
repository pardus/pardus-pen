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
    endPos = event->pos();
    update();
}

void ScreenshotWidget::mouseMoveEvent(QMouseEvent *event) {
    endPos = event->pos();
    update();
}

void ScreenshotWidget::mouseReleaseEvent(QMouseEvent *event) {

    endPos = event->pos();
    QPoint pos1 = startPos;
    QPoint pos2 = endPos;
    hide();
    // reset surface
    startPos = QPoint(-1, -1);
    endPos = QPoint(-1, -1);
    repaint();
    QTimer::singleShot(300, this, [=]() {
        cropScreenshot(pos1, pos2);
    });
}

void ScreenshotWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);

    // background
    QColor color = drawing->penColor;
    color.setAlpha(69);
    painter.setBrush(QBrush(color));
    painter.setCompositionMode(QPainter::CompositionMode_Source);


    painter.drawRect(QRect(0,0,geometry().width(), geometry().height()));
    // selection
    QRect rect(startPos, endPos);
    painter.setBrush(QBrush(QColor(0,0,0,0)));
    painter.drawRect(rect.normalized());
}

void ScreenshotWidget::cropScreenshot(QPoint startPos, QPoint endPos) {
    QScreen *screen = QGuiApplication::primaryScreen();
    QPixmap originalPixmap = screen->grabWindow(0);
    QRect cropRect(startPos, endPos);
    QPixmap croppedPixmap = originalPixmap.copy(cropRect.normalized());
    crop_signal(croppedPixmap);
}

