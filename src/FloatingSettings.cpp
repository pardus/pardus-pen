#include "FloatingSettings.h"
extern int screenWidth;
extern int screenHeight;

#define padding 8

extern "C" {
#include "settings.h"
}


#include <QMap>

class SettingsPages {
public:
    void addPage(qint64 id, QWidget *data) {
        values[id] = data;
    }

    QWidget * getPage(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            return NULL;
        }
    }

private:
    QMap<qint64, QWidget*> values;
};

SettingsPages settingsPages;

FloatingSettings::FloatingSettings(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    setLayout(layout);
    QString style = QString(
    "border-radius:3px;"
    "background-color: #80808080;");
    layout->setSpacing(padding);
    layout->setContentsMargins(padding, padding, padding, padding);
    setStyleSheet(style);
    cur_height = padding;
    cur_width = padding;
}

void FloatingSettings::addPage(QWidget *widget) {
    settingsPages.addPage(num_of_item, widget);
    layout->addWidget(widget);
    widget->hide();
    num_of_item++;
}

void FloatingSettings::setPage(int num){
    for(int i=0;i<num_of_item;i++){
        settingsPages.getPage(i)->hide();
    }
    settingsPages.getPage(num)->show();
    setFixedSize(
        settingsPages.getPage(num)->size().width() + padding * 2,
        settingsPages.getPage(num)->size().height() + padding * 2
    );
}
