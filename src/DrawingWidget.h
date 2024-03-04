#ifndef DRAWINGWIDGET_H
#define DRAWINGWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <iostream>


class DrawingWidget : public QWidget {
public:
    explicit DrawingWidget(QWidget *parent = nullptr);
    ~DrawingWidget(); // Destructor

    QImage image;
    QPoint lastPoint;
    int penWidth;
    int eraserWidth;
    QColor penColor;
    void initializeImage(const QSize &size);
    void drawLineTo(const QPoint &endPoint);
    void setEraser(bool enabled);

protected:
    bool drawing;
    bool eraser;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent * ev);
};

QColor convertColor(QColor color);

#endif // DRAWINGWIDGET_H

