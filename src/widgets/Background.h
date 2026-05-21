#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QWidget>
#include <QResizeEvent>
#include <QScreen>
#include <QApplication>
#include <QMainWindow>

class Background;

extern Background *background;
extern QPushButton* pageLabel;
extern QWidget *bgMenu;

void setupBackground();
void setHideMainWindow(bool status);

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
