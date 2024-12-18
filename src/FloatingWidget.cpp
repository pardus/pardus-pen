#include "FloatingWidget.h"


#include "tools/tools.h"

int new_x = 0;
int new_y = 0;

extern float scale;

extern QMainWindow* tool2;

#define padding 8*scale


extern "C" {
#include "settings.h"
}

#ifdef QT5
#define globalPosition globalPos
#endif

FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    if(is_vertical){
        layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    } else {
        layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    }
    new_x = get_int((char*)"cur-x");
    new_y = get_int((char*)"cur-y");
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    setLayout(layout);
    QString style = QString(
    "QWidget {"
        "border-radius: 13px;"
        "background-color: #cc939393;"
    "}");
    setStyleSheet(style);
    cur_height = padding;
    cur_width = padding;

}

void FloatingWidget::setMainWindow(QWidget *widget) {
    mainWindow = (QMainWindow*)widget;
}

void FloatingWidget::setSettings(QWidget *widget) {
    floatingSettings = (FloatingSettings*)widget;
}

void FloatingWidget::addWidget(QString name, QWidget *widget) {
    layout->addWidget(widget);
    widgets.insert(name, widget);
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
    moveAction();
}


static int new_xx = 0, new_yy = 0;

void FloatingWidget::moveAction(){
        if(tool2 == nullptr){
            return;
        }
        if (new_x < 0) {
            new_x = 0;
        }if (new_y < 0) {
            new_y = 0;
        }
        int max_width = mainWindow->geometry().width();
        int max_height = mainWindow->geometry().height();
        if(tool2 != nullptr){
            max_width = QGuiApplication::primaryScreen()->size().width();
            max_height = QGuiApplication::primaryScreen()->size().height();
        }
        if (new_x >  max_width- cur_width) {
            new_x = max_width - cur_width;
        }if (new_y > max_height - cur_height) {
            new_y = max_height - cur_height;
        }
        if(tool2 != nullptr){
            tool2->move(new_x, new_y);
        } else {
            move(new_x, new_y);
        }
        if(floatingSettings != NULL){
            if(is_vertical){
                new_xx = new_x+padding+cur_width;
                if(new_xx  > mainWindow->geometry().width() - floatingSettings->cur_width){
                    new_xx = new_x - padding - floatingSettings->cur_width;
                }
                new_yy = new_y;
                if (new_yy > mainWindow->geometry().height() - floatingSettings->cur_height) {
                    new_yy = mainWindow->geometry().height() - floatingSettings->cur_height;
                }

            } else {
                new_xx = new_x;
                if (new_xx > mainWindow->geometry().width() - floatingSettings->cur_width) {
                    new_xx = mainWindow->geometry().width() - floatingSettings->cur_width;
                }
                new_yy = new_y + cur_height + padding;
                if(new_yy  > mainWindow->geometry().height() - floatingSettings->cur_height){
                    new_yy = new_y - padding - floatingSettings->cur_height;
                }
            }
            if(tool2 != nullptr){
                tool2->move(new_xx, new_yy + padding);
            } else {
                floatingSettings->move(new_xx, new_yy + padding);
            }
        }
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
