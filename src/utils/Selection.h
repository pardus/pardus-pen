#ifndef SELECTION_H
#define SELECTION_H

#include <QWidget>
#include <QMouseEvent>

#define DRAG 0
#define RESIZE 1

class MovableWidget : public QWidget {

public:
    QImage image;
    QLabel* crop;
    explicit MovableWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint lastMousePosition;
    int mode = 0;
};

#endif
