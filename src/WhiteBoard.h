#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QApplication>

#define TRANSPARENT 0
#define WHITE 1
#define BLACK 2

class WhiteBoard : public QWidget {
public:
    WhiteBoard(QWidget *parent = nullptr);
    int getType();
    void setType(int type);
private:
    void enable();
    void enableDark();
    void disable();
    int type = 0;
};

#endif // FLOATINGWIDGET_H
