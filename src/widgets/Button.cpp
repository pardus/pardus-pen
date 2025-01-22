#include <QMap>

#include "Button.h"
#include <stdio.h>

extern float scale;

#define padding 3*scale
#define butsize 48*scale

typedef struct {
    qint64 key;
    qint64 modifier;
} Shortcut;

static QMap<QPushButton*, ButtonEvent> events;
static QMap<QPushButton*, Shortcut> shortcuts;

QPushButton* create_button_text(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton(name);
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
        events[button] = event;
    }
    QFont font = button->font();
    font.setPointSize(18*scale);
    button->setFont(font);
    return button;
}
QPushButton* create_button(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
        events[button] = event;
    }
    set_icon(name, button);
    QFont font = button->font();
    font.setPointSize(18*scale);
    button->setFixedSize(butsize+padding, butsize+padding);
    button->setFont(font);
    return button;
}

void set_shortcut(QPushButton *button, qint64 key, qint64 modifier){
    Shortcut s;
    s.key = key;
    s.modifier = modifier;
    shortcuts[button] = s;
}
void do_shortcut(qint64 key, qint64 modifier){
    QPushButton *button = NULL;
    for (auto i = shortcuts.cbegin(), end = shortcuts.cend(); i != end; ++i){
        Shortcut s = i.value();
        if(s.key == key && s.modifier == modifier){
            button = i.key();
            break;
        }
    }
    if(events.contains(button)){
        ButtonEvent ev = events[button];
        ev();
    }

}

void set_icon(const char* name, QPushButton * button) {
    QIcon icon = QIcon(name);
    QPixmap pixmap = icon.pixmap(QSize(butsize, butsize));
    button->setIcon(icon);
    button->setIconSize(QSize(butsize, butsize));
    button->setFlat(true);
}
