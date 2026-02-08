#ifndef SCREENSHOTWIDGET_H
#define SCREENSHOTWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QMouseEvent>

class ScreenshotWidget : public QWidget {

public:
    ScreenshotWidget(QWidget *parent = nullptr);
    void (*crop_signal)(QPixmap pixmap);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void cropScreenshot(QPoint startPos, QPoint endPos);

    QPoint startPos;
    QPoint endPos;
    bool selecting;
};

#endif
