#include "FloatingWidget.h"
int cur_height = 0;
int cur_width = 0;
FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
}

void FloatingWidget::setWidget(QWidget *widget) {
    cur_height += widget->size().height();
    cur_width += widget->size().width();
    setFixedSize(cur_height, cur_width);
    layout->addWidget(widget);
}

void FloatingWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = geometry().center();
        event->accept();
    }
}

void FloatingWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}
