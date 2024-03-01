#include "FloatingWidget.h"
int cur_height = 0;
int cur_width = 0;
int screenWidth = 0;
int screenHeight = 0;

FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen *screen : screens) {
        screenWidth  += screen->geometry().width();
        screenHeight += screen->geometry().height();
        break;
    }
    setLayout(layout);
}

void FloatingWidget::setWidget(QWidget *widget) {
    cur_height += widget->size().height();
    if (cur_width < widget->size().width()) {
        cur_width = widget->size().width();
    }
    setFixedSize(cur_width+5, cur_height+5);
    layout->addWidget(widget);
}

void FloatingWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        int new_x = event->globalPos().x() - (cur_width / 2);
        int new_y = event->globalPos().y() - (cur_height / 2);
        if (new_x < 0) {
            new_x = 0;
        }if (new_y < 0) {
            new_y = 0;
        }if (new_x > screenWidth - cur_width) {
            new_x = screenWidth - cur_width;
        }if (new_y > screenHeight - cur_height) {
            new_y = screenHeight - cur_height;
        }
        move(new_x, new_y);
        event->accept();
    }
}
