#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QApplication>
#include <QMainWindow>

#define TRANSPARENT 0
#define WHITE 1
#define BLACK 2

#define NONE 0
#define SQUARES 1
#define LINES 2
#define ISOMETRIC 3
#define MUSIC 4
#define TURKIYE 90
#define WORLD 23
#define CUSTOM 31

class WhiteBoard : public QWidget {
public:
    WhiteBoard(QWidget *parent = nullptr);
    int getType();
    int getOverlayType();
    void setType(int type);
    void setOverlayType(int type);
    void drawAction(QWidget* widget);
    void setImage(QImage image);
    void drawSquarePaper();
    void drawLinePaper();
    void drawIsometricPaper();
    QMainWindow *mainWindow;
    void drawMusicPaper();
    QImage backgroundImage;
private:
    QColor background;
    QColor lineColor;
    int overlayType = 0;
    int type = 0;
    QPainter painter;
    float gridSize;
    void paintEvent(QPaintEvent *event) override ;
};

#endif // FLOATINGWIDGET_H
