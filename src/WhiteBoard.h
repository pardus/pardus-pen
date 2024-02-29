#ifndef WHITEBOARD_H
#define WHITEBOARD_H

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QDesktopWidget>
#include <QApplication>

class WhiteBoard : public QWidget {
public:
    WhiteBoard(QWidget *parent = nullptr);
    void enable();
    void enableDark();
    void disable();
};

#endif // FLOATINGWIDGET_H
