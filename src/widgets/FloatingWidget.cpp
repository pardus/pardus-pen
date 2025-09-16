#include <QLabel>
#include "FloatingWidget.h"


#include "../tools.h"


extern float scale;

extern QMainWindow* tool2;

#define padding 8*scale


#ifdef QT5
#define globalPosition globalPos
#endif

FloatingWidget::FloatingWidget(QWidget *parent) : QWidget(parent) {
    tool = (QMainWindow*)parent;
    layout = new QGridLayout();
    new_x = get_int("cur-x");
    new_y = get_int("cur-y");
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    QString style = QString(
    "QWidget {"
        "border-radius: "+QString::number(13*scale)+"px;"
        "background-color: #f3232323;"
    "}");
    setLayout(layout);
    setStyleSheet(style);
    cur_height = padding;
    for(int i=0;i<num_of_rows;i++){
        QLabel *label = new QLabel();
        label->setFixedSize(butsize/2, butsize/2);
        label->setStyleSheet(QString("background-color: none;"));
        widgets.insert(i, label);
    }

}


void FloatingWidget::setMainWindow(QWidget *widget) {
    mainWindow = (QMainWindow*)widget;
}

void FloatingWidget::setSettings(QWidget *widget) {
    floatingSettings = (FloatingSettings*)widget;
}

void FloatingWidget::addWidget(QWidget *widget) {
    widgets.insert(num_of_rows + num_of_item, widget);
    num_of_item++;
    setVertical(is_vertical);
    moveAction();
}
void FloatingWidget::setVertical(bool state) {
    // remove all items
    for(int i=0;i<layout->rowCount();i++){
        for(int j=0;j<layout->rowCount();j++){
            QLayoutItem* item = layout->itemAtPosition(i, j);
            if (!item) continue;

            if (item->widget()) {
                layout->removeWidget(item->widget());
            } else {
                layout->removeItem(item);
            }
        }
    }
    // add items
    int item = 0;
    int height = (num_of_item / num_of_rows)*(butsize+padding) + 2*padding + butsize / 2;
    int width = num_of_rows*(butsize+padding) + 2*padding;
    for (qint64 i=0;i<num_of_item + num_of_rows;i++) {
        int row = (int)item / num_of_rows;
        int column = (int)item % num_of_rows;
        item++;
        if(state) {
            layout->addWidget(widgets[i], row, column);
            setFixedSize(width, height);
            if(tool != nullptr){
                tool->setFixedSize(width, height);
            }
            } else {
            layout->addWidget(widgets[i], column, row, Qt::AlignCenter);
            setFixedSize(height, width);
            if(tool != nullptr){
                tool->setFixedSize(height, width);
            }
        }
    }
}


void FloatingWidget::moveAction(){
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
        if (new_x >  max_width- size().width()) {
            new_x = max_width - size().width();
        }if (new_y > max_height - size().height()) {
            new_y = max_height - size().height();
        }
        if(tool != nullptr){
            tool->move(new_x, new_y);
        } else {
            move(new_x, new_y);
        }
        if(floatingSettings != NULL){
            new_xx = new_x+padding+size().width();
            if(new_xx  > max_width - floatingSettings->cur_width){
                new_xx = new_x - padding - floatingSettings->cur_width;
            }
            new_yy = new_y;
            if (new_yy > max_height - floatingSettings->cur_height) {
                new_yy = max_height - floatingSettings->cur_height;
            }
             if(tool2 != nullptr){
                tool2->move(new_xx, new_yy);
            } else {
                floatingSettings->move(new_xx, new_yy);
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
    set_int("cur-x", new_x);
    set_int("cur-y", new_y);
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
