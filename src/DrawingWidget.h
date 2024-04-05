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


#define ERASER 0
#define PEN 1
#define MARKER 2

class DrawingWidget : public QWidget {
public:
    explicit DrawingWidget(QWidget *parent = nullptr);
    ~DrawingWidget(); // Destructor

    QImage image;
    QPoint lastPoint;
    QColor penColor;
    QWidget* floatingSettings;
    int penSize[3];
    void initializeImage(const QSize &size);
    void drawLineTo(const QPoint &endPoint);
    void goPrevious();
    void goNext();
    void goPreviousPage();
    void goNextPage();
    void clear();
    int penType;
    void syncPageType(int type);
    int getPageNum();
    bool isBackAvailable();
    bool isNextAvailable();

protected:
    bool drawing;
    bool eraser;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void drawLineToFunc(const QPoint startPoint, const QPoint endPoint, qreal pressure);
    void loadImage(int num);
    bool event(QEvent * ev);
    QPainter painter;
};

QColor convertColor(QColor color);

#endif // DRAWINGWIDGET_H

