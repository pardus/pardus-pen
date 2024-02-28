#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>

class QLabel;

class FloatingWidget : public QWidget {
public:
    FloatingWidget(QWidget *parent = nullptr);
    void setWidget(QWidget *widget);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint dragPosition;
    QLabel *label;
    QVBoxLayout *layout;
};

#endif // FLOATINGWIDGET_H
