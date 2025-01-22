#include <QMap>

#include "Button.h"
#include <stdio.h>

extern float scale;

#define padding 3*scale
#define butsize 48*scale

typedef struct {
    int key;
    int modifier;
} Shortcut;

static QMap<const char*, QPushButton*> buttons;
static QMap<const char*, ButtonEvent> events;
static QMap<const char*, Shortcut> shortcuts;

QPushButton* create_button_text(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton(name);
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
        events[name] = event;
    }
    QFont font = button->font();
    font.setPointSize(18*scale);
    button->setFont(font);
    buttons[name] = button;
    return button;
}
QPushButton* create_button(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    if(event) {
        QObject::connect(button, &QPushButton::clicked, event);
        events[name] = event;
    }
    set_icon(name, button);
    QFont font = button->font();
    font.setPointSize(18*scale);
    button->setFixedSize(butsize+padding, butsize+padding);
    button->setFont(font);
    buttons[name] = button;
    return button;
}

void set_shortcut(QPushButton *button, int key, int modifier){
    char* name;
    for (auto i = buttons.cbegin(), end = buttons.cend(); i != end; ++i){
        if(i.value() == button){
            name = (char*) i.key();
            break;
        }
    }
    Shortcut s;
    s.key = key;
    s.modifier = modifier;
    shortcuts[name] = s;
}
void do_shortcut(int key, int modifier){
    char* name;
    for (auto i = shortcuts.cbegin(), end = shortcuts.cend(); i != end; ++i){
        Shortcut s = i.value();
        if(s.key == key && s.modifier == modifier){
            name = (char*)i.key();
            break;
        }
    }
    if(events.contains(name)){
        ButtonEvent ev = events[name];
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
