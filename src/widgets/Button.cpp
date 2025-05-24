#include <QMap>

#include "Button.h"
#include <stdio.h>
#include <QColor>

extern float scale;

typedef struct {
    qint64 key;
    qint64 modifier;
} Shortcut;

static QMap<QPushButton*, ButtonEvent> events;
static QMap<QPushButton*, Shortcut> shortcuts;

QPushButton* create_button_text(const char* name, ButtonEvent event) {
    QPushButton* button = new QPushButton(name);
    if(event) {
        events[button] = [=]() {
            event();
            updateGui();
        };
        QObject::connect(button, &QPushButton::clicked, events[button]);
    }
    button->setStyleSheet(
        "background-color: none;"
    );

    return button;
}
QPushButton* create_button(int id, ButtonEvent event) {
    QPushButton* button = new QPushButton("");
    if(event) {
        events[button] = [=]() {
            event();
            updateGui();
        };
        QObject::connect(button, &QPushButton::clicked, events[button]);
    }
    set_icon(get_icon_by_id(id), button);
    button->setFixedSize(butsize, butsize);
    button->setStyleSheet(QString("background-color: none;"));
    return button;
}
QPushButton* create_color_button(QColor color){
    QPushButton* button = new QPushButton();
    button->setFixedSize(butsize, butsize);
    button->setStyleSheet(QString(
        "background-color: "+color.name()+";"
        "border-radius: 12px;"
    ));
    QObject::connect(button, &QPushButton::clicked, [=]() {
        drawing->penColor = color;
        set_string("color", drawing->penColor.name());
        if(drawing->getPen() == ERASER){
            drawing->setPen(PEN);
        }
        floatingSettings->setHide();
        updateGui();
    });
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

static QIcon combineIcons(const QIcon &icon1, const QIcon &icon2) {
    // Create a QPixmap to hold the combined image
    QPixmap combinedPixmap(64, 64); // Adjust size as needed
    combinedPixmap.fill(Qt::transparent); // Fill with transparent background

    QPainter painter(&combinedPixmap);

    // Draw the first icon
    QPixmap pixmap1 = icon1.pixmap(64, 64); // Get the pixmap of the first icon
    painter.drawPixmap(0, 0, pixmap1); // Draw it at the top-left corner

    // Draw the second icon
    QPixmap pixmap2 = icon2.pixmap(32, 32); // Get the pixmap of the second icon
    painter.drawPixmap(32 - 3, 32 - 3, pixmap2); // Draw it at the center (adjust as needed)

    painter.end(); // End the painter

    return QIcon(combinedPixmap); // Create a new QIcon from the combined pixmap
}


void set_icon(const char* name, QPushButton * button) {
    QIcon icon = QIcon(name);
    QPixmap pixmap = icon.pixmap(QSize(butsize, butsize));
    button->setIcon(icon);
    button->setIconSize(QSize(butsize - padding, butsize - padding));
    button->setFlat(true);
}

void set_icon_combined(const char* name, const char* subname, QPushButton * button){
    QIcon icon = combineIcons(QIcon(name), QIcon(subname));
    QPixmap pixmap = icon.pixmap(QSize(butsize, butsize));
    button->setIcon(icon);
    button->setIconSize(QSize(butsize - padding, butsize - padding));
    button->setFlat(true);
}

