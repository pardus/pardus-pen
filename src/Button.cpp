#include "Button.h"
#include <stdio.h>

#define padding 3
extern int screenWidth;
extern int screenHeight;

QPushButton* create_button(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    QObject::connect(button, &QPushButton::clicked, event);
    set_icon(name, button);
    return button;
}

void set_icon(const char* name, QPushButton * button) {
    QIcon icon = QIcon(name);
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(screenHeight/23, screenHeight/23)));
    button->setIcon(icon);
    button->setIconSize(pixmap.rect().size());
    button->setFixedSize(screenHeight/23+padding, screenHeight/23+padding);
    button->setFlat(true);
}
