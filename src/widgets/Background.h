#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QApplication>
#include <QMainWindow>

class Background : public QWidget {
public:
    Background(QWidget *parent = nullptr);
    QMainWindow *mainWindow;
    QImage image;
    void applyImage(QImage img);
private:
    QPainter painter;
    void paintEvent(QPaintEvent *event) override ;
};

#endif // BACKGROUND_H
