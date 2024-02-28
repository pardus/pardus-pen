#include "Button.h"
#include <stdio.h>

QPushButton* create_button(ButtonEvent event) {
    QPushButton* eraserButton = new QPushButton("");
    QObject::connect(eraserButton, &QPushButton::clicked, event);
    return eraserButton;
}
