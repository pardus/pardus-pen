#include "Button.h"
#include <stdio.h>

#define padding 3
#define butsize 48

QPushButton* create_button_text(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton(name);
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
    }
    QFont font = button->font();
    font.setPointSize(18);
    button->setFont(font);
    return button;
}
QPushButton* create_button(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
    }
    set_icon(name, button);
    QFont font = button->font();
    font.setPointSize(18);
    button->setFixedSize(butsize+padding, butsize+padding);
    button->setFont(font);
    return button;
}

void set_icon(const char* name, QPushButton * button) {
    QIcon icon = QIcon(name);
    QPixmap pixmap = icon.pixmap(QSize(butsize, butsize));
    button->setIcon(icon);
    button->setIconSize(QSize(butsize, butsize));
    button->setFlat(true);
}
