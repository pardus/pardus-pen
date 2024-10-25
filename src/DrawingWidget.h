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

#include <QtWidgets>

#include <QImage>
#include <QPainter>


#define ERASER 0
#define PEN 1
#define MARKER 2


#define LINE 0
#define CIRCLE 1
#define SPLINE 2
#define RECTANGLE 3

class DrawingWidget : public QWidget {
public:
    explicit DrawingWidget(QWidget *parent = nullptr);
    ~DrawingWidget(); // Destructor

    QImage image;
    QPointF lastPoint;
    QPointF firstPoint;
    QColor penColor;
    QWidget* floatingSettings;
    int penSize[3];
    void initializeImage(const QSize &size);
    void drawLineTo(const QPointF &endPoint);
    void goPrevious();
    void goNext();
    void goPreviousPage();
    void goNextPage();
    void clear();
#ifdef LIBARCHIVE
    void saveAll(QString filename);
    void loadArchive(const QString& filename);
#endif
    int penType;
    int penStyle;
    void syncPageType(int type);
    int getPageNum();
    bool isBackAvailable();
    bool isNextAvailable();
    void loadImage(int num);

protected:
    bool drawing = false;
    QImage imageBackup;
    bool eraser;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateCursorMouse(qint64 i, QPoint pos);
    void updateCursorMouse(qint64 i, QPointF pos);
    void drawLineToFunc(const QPointF startPoint, const QPointF endPoint, qreal pressure);
    bool event(QEvent * ev);
    QPainter painter;
};

QColor convertColor(QColor color);
void qImageToFile(const QImage& image, const QString& filename);

#endif // DRAWINGWIDGET_H

