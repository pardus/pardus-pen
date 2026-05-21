#ifndef BUTTON_H
#define BUTTON_H

#include <QPushButton>
#include <QWindow>
#include <QColor>
#include <QMap>
#include <functional>

#include "../constants.h"

class QSlider;

extern QMap<qint64, QPushButton*> penButtons;
extern QMap<qint64, QPushButton*> toolButtons;
extern QSlider *thicknessSlider;
extern char* get_icon_by_id(int id);
extern QString get_overlay_by_id(int id);
extern int get_id_by_overlay(QString foverlay);

typedef std::function<void()> ButtonEvent;

QPushButton* create_button(int id, ButtonEvent event);
QPushButton* create_button_text(const char* icon, ButtonEvent event);
QPushButton* create_color_button(QColor color, bool read_only, const char* name);

void set_icon(const char* name, QPushButton* button);
void set_icon_combined(const char* name, const char* subname, QPushButton * button);

void set_shortcut(QPushButton *button, qint64 shortcut, qint64 modifier);
void do_shortcut(qint64 shortcut, qint64 modifier);

#endif // BUTTON_H
