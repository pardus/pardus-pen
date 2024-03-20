#include "FloatingSettings.h"
extern int screenWidth;
extern int screenHeight;

#define padding 8

extern "C" {
#include "settings.h"
}


FloatingSettings::FloatingSettings(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
    QString style = QString(
    "border-radius:3px;"
    "background-color: #80808080;");
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    setStyleSheet(style);
    cur_height = padding;
    cur_width = padding;
}

void FloatingSettings::setWidget(QWidget *widget) {
    cur_height += widget->size().height() + padding;
    if (cur_width < widget->size().width()) {
        cur_width = widget->size().width() + padding*2;
    }
    num_of_item++;
    setFixedSize(cur_width, cur_height);
    layout->addWidget(widget);
}
