#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QWindow>

typedef std::function<void()> ButtonEvent;

QPushButton* create_button(const char* icon, ButtonEvent event);

#endif // BUTTON_H
