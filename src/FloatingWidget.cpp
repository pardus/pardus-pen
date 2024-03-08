#include "FloatingWidget.h"
int cur_height = 0;
int cur_width = 0;
extern int screenWidth;
extern int screenHeight;

int new_x;
int new_y;
#define padding 8

extern "C" {
#include "settings.h"
}


FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
    QString style = QString(
    "border-radius:3px;"
    "background-color: #80808080;");
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    setStyleSheet(style);
    cur_height = padding;
}


int num_of_item = 0;
void FloatingWidget::setWidget(QWidget *widget) {
    cur_height += widget->size().height() + padding;
    if (cur_width < widget->size().width()) {
        cur_width = widget->size().width() + padding*2;
    }
    num_of_item++;
    setFixedSize(cur_width, cur_height);
    layout->addWidget(widget);
    new_x = get_int((char*)"cur-x");
    new_y = get_int((char*)"cur-y");
    moveAction(new_x, new_y);
}


void FloatingWidget::mouseReleaseEvent(QMouseEvent *event) {
        set_int((char*)"cur-x", new_x);
        set_int((char*)"cur-y", new_y);
}

void FloatingWidget::moveAction(int new_x, int new_y){
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
}

void FloatingWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        new_x = event->globalPos().x() - (cur_width / 2);
        new_y = event->globalPos().y() - (cur_height / 2);
        moveAction(new_x, new_y);
        event->accept();
    }
}
