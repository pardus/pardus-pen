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

#include "../utils/Selection.h"
#include "FloatingSettings.h"

#define PRESS 0
#define MOVE 1
#define RELEASE 2

class ValueStorage {
public:
    int size() {
        return values.size();
    }

    void clear() {
        return values.clear();
    }

    void saveValue(qint64 id, QPointF data) {
        values[id] = data;
    }
    QPointF last() {
        return loadValue(values.size()-1);
    }

    QPointF first() {
        return loadValue(0);
    }

    QPointF loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            // -1 -1 means disable drawing
            return QPointF(-1,-1);
        }
    }
    QMap<qint64, QPointF> values;
};

class GeometryStorage {
public:
    QPointF point;
    QPointF point_last;

    void addValue(qint64 id, QPointF data) {
        values[id].saveValue(values[id].size(), data);
    }

    void saveValue(qint64 id, qint64 id2, QPointF data) {
        values[id].saveValue(id2, data);
    }

    QPointF last(qint64 id){
        return load(id).last();
    }

    QPointF first(qint64 id){
        return load(id).first();
    }

    int size(qint64 id) {
        return values[id].size();
    }

    ValueStorage load(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            // -1 -1 means disable drawing
            ValueStorage v;
            return v;
        }
    }
    void clear(qint64 id) {
        return values[id].clear();
    }
    void clearAll() {
        for (auto it = values.begin(); it != values.end(); ++it) {
            values[it.key()].clear();
        }
        values.clear();
    }
    QMap<qint64, ValueStorage> values;
    QMap<qint64, QPointF> last_begin;
    QMap<qint64, QPointF> last_end;

};

class DrawingWidget : public QWidget {
public:
    explicit DrawingWidget(QWidget *parent = nullptr);

    QImage image;
    QColor penColor;
    MovableWidget* cropWidget;
    int penSize[10];
    void initializeImage(const QSize &size);
    void goPrevious();
    void goPage(int i);
    void goNext();
    void goPreviousPage();
    void goNextPage();
    void clear();
    void clearAll();
#ifdef LIBARCHIVE
    void saveAll(QString filename);
    void loadArchive(const QString& filename);
#endif
    void syncPageType(int type);
    int getPageNum();
    bool isBackAvailable();
    bool isNextAvailable();
    void loadImage(int num);
    void mergeSelection();
    void clearSelection();
    void addImage(QImage img);
    void setPen(int type);
    int getPen();
    void setPenStyle(int type);
    int getPenStyle();
    void setLineStyle(int type);
    int getLineStyle();
    void drawArrow(QPainter& painter, QPointF start, QPointF end);
    void eventHandler(int source, int type, int id, QPointF pos, float pressure);

protected:
    float fpressure;
    bool eraser;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateCursorMouse(qint64 i, QPoint pos);
    void updateCursorMouse(qint64 i, QPointF pos);
    void createSelection(int source);
    void drawLineToFunc(qint64 id, qreal pressure);
    void drawFunc(qint64 id, qreal pressure);
    void selectionDraw(QPointF startPoint, QPointF endPoint);
    void addPoint(int id, QPointF data);
    bool event(QEvent * ev) override;
    int penType;
    int penStyle;
    int lineStyle;
    GeometryStorage geo;
    QPainter painter;
};

QColor convertColor(QColor color);
void qImageToFile(const QImage& image, const QString& filename);

#endif // DRAWINGWIDGET_H

