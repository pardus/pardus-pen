#include "FloatingSettings.h"
extern int screenWidth;
extern int screenHeight;

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
    "QWidget {"
        "border-radius:3px;"
        "background-color: #80808080;"
    "}");
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setStyleSheet(style);
    cur_height = 0;
    cur_width = 0;
}

void FloatingSettings::addPage(QWidget *widget) {
    settingsPages.addPage(num_of_item, widget);
    layout->addWidget(widget);
    widget->hide();
    num_of_item++;
}

void FloatingSettings::setPage(int num){
    if(settingsPages.getPage(num)->isVisible()){
        settingsPages.getPage(num)->hide();
        hide();
        return;
    }
    for(int i=0;i<num_of_item;i++){
        settingsPages.getPage(i)->hide();
    }
    settingsPages.getPage(num)->show();
    cur_width = settingsPages.getPage(num)->size().width();
    cur_height = settingsPages.getPage(num)->size().height();
    setFixedSize(cur_width, cur_height);
    show();
}
