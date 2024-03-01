#ifndef FLOATINGWIDGET_H
#define FLOATINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>

class QLabel;

class FloatingWidget : public QWidget {
public:
    FloatingWidget(QWidget *parent = nullptr);
    void setWidget(QWidget *widget);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint dragPosition;
    QLabel *label;
    QVBoxLayout *layout;
};

#endif // FLOATINGWIDGET_H
