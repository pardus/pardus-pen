#include "FloatingWidget.h"
#include "FloatingSettings.h"

int new_x;
int new_y;

#define padding 8


extern "C" {
#include "settings.h"
}

#ifdef QT5
#define globalPosition globalPos
#endif

FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    mainWindow = (QMainWindow*)parent;
    is_vertical = get_bool((char*)"is-vertical");
    if(is_vertical){
        layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    } else {
        layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    }
    setLayout(layout);
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    QString style = QString(
    "QWidget {"
        "border-radius: 13px;"
        "background-color: #cc939393;"
    "}");
    setStyleSheet(style);
    cur_height = padding;
}


void FloatingWidget::setSettings(QWidget *widget) {
    floatingSettings = (FloatingSettings*)widget;
}

void FloatingWidget::setWidget(QWidget *widget) {
    if(is_vertical){
        cur_height += widget->size().height() + padding;
        if (cur_width < widget->size().width()) {
            cur_width = widget->size().width() + padding*2;
        }
    } else {
        cur_width += widget->size().width() + padding;
        if (cur_height < widget->size().height()) {
            cur_height = widget->size().height() + padding*2;
        }
    }
    num_of_item++;
    setFixedSize(cur_width, cur_height);
    layout->addWidget(widget);
    moveAction();
}

void FloatingWidget::mousePressEvent(QMouseEvent *event) {
    offset_x = abs(event->globalPosition().x() - new_x);
    offset_y = abs(event->globalPosition().y() - new_y);
}

void FloatingWidget::mouseReleaseEvent(QMouseEvent *event) {
    (void)(event); // fix unused warning
    offset_x =-1;
    offset_y =-1;
    set_int((char*)"cur-x", new_x);
    set_int((char*)"cur-y", new_y);
}
static int new_xx, new_yy;

void FloatingWidget::moveAction(){
        if (new_x < 0) {
            new_x = 0;
        }if (new_y < 0) {
            new_y = 0;
        }if (new_x > mainWindow->geometry().width() - cur_width) {
            new_x = mainWindow->geometry().width() - cur_width;
        }if (new_y > mainWindow->geometry().height() - cur_height) {
            new_y = mainWindow->geometry().height() - cur_height;
        }
        move(new_x, new_y);
        if(floatingSettings != NULL){
            if(is_vertical){
                new_xx = new_x+padding+cur_width;
                if(new_xx  > mainWindow->geometry().width() - floatingSettings->cur_width){
                    new_xx = new_x - padding - floatingSettings->cur_width;
                }
                new_yy = new_y + (cur_height / num_of_item) * settingsOffset;
                if (new_yy > mainWindow->geometry().height() - floatingSettings->cur_height) {
                    new_yy = mainWindow->geometry().height() - floatingSettings->cur_height;
                }

            } else {
                new_xx = new_x + (cur_width / num_of_item) * settingsOffset;
                if (new_xx > mainWindow->geometry().width() - floatingSettings->cur_width) {
                    new_xx = mainWindow->geometry().width() - floatingSettings->cur_width;
                }
                new_yy = new_y + cur_height + padding;
                if(new_yy  > mainWindow->geometry().height() - floatingSettings->cur_height){
                    new_yy = new_y - padding - floatingSettings->cur_height;
                }
            }
            floatingSettings->move(new_xx, new_yy + padding);
        }
}

void FloatingWidget::setFloatingOffset(int offset){
    settingsOffset = offset;
    moveAction();
}

void FloatingWidget::mouseMoveEvent(QMouseEvent *event) {
    if(offset_x < 0 || offset_y < 0){
        return;
    }
    if (event->buttons() & Qt::LeftButton) {
        new_x = event->globalPosition().x() - offset_x;
        new_y = event->globalPosition().y() - offset_y;
        moveAction();
        event->accept();
    }
}
