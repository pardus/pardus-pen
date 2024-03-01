#include "Button.h"
#include <stdio.h>

#define padding 3

QPushButton* create_button(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    QObject::connect(button, &QPushButton::clicked, event);
    set_icon(name, button);
    return button;
}

void set_icon(const char* name, QPushButton * button) {
    QIcon icon = QIcon::fromTheme(name);
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(48, 48)));
    button->setIcon(icon);
    button->setIconSize(pixmap.rect().size());
    button->setFixedSize(48+padding, 48+padding);
    button->setFlat(true);
}
