#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QWindow>
#include <functional>


typedef std::function<void()> ButtonEvent;

QPushButton* create_button(const char* icon, ButtonEvent event);
QPushButton* create_button_text(const char* icon, ButtonEvent event);

void set_icon(const char* name, QPushButton* button);

#endif // BUTTON_H
