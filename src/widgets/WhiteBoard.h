#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include "../constants.h"

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QApplication>
#include <QMainWindow>

class WhiteBoard : public QWidget {
public:
    WhiteBoard(QWidget *parent = nullptr);
    int getType();
    int getOverlayType();
    void setType(int type);
    void setOverlayType(int type);
    void setImage(QImage image);
    void drawSquarePaper();
    void drawLinePaper();
    void drawIsometricPaper();
    QMainWindow *mainWindow;
    void drawMusicPaper();
    QMap<qint64, int> ratios;
    QMap<qint64, int> rotates;
    QMap<qint64, QPixmap> backgrounds;
    QColor background;
    QColor lineColor;
    void updateTransform();
    int overlayType = BLANK;
    int type = WHITE;
private:
    QPainter painter;
    float gridSize;
    QImage transformImage;
    int ow, oh;
    void paintEvent(QPaintEvent *event) override;
};

#endif // FLOATINGWIDGET_H
