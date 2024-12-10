#ifndef SELECTION_H
#define SELECTION_H

#include <QWidget>
#include <QMouseEvent>

class MovableWidget : public QWidget {

public:
    explicit MovableWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint lastMousePosition;
    bool dragging = false;
};

#endif
