#include "FloatingSettings.h"

#include <QMainWindow>

#include "../tools.h"

extern QMainWindow* tool2;

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
        "border-radius:13px;"
        "background-color: #cc939393;"
        "}"
    );
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setStyleSheet(style);
    cur_height = 0;
    cur_width = 0;
    setFixedSize(cur_width, cur_height);
}

void FloatingSettings::addPage(QWidget *widget) {
    settingsPages.addPage(num_of_item, widget);
    layout->addWidget(widget);
    widget->hide();
    num_of_item++;
}

void FloatingSettings::reload(){
    if(num_of_item <= current_page || current_page < 0){
        return;
    }
    settingsPages.getPage(current_page)->show();
    settingsPages.getPage(current_page)->adjustSize();
    cur_width = settingsPages.getPage(current_page)->size().width();
    cur_height = settingsPages.getPage(current_page)->size().height();
    setFixedSize(cur_width, cur_height);
    if(tool2 != nullptr) {
        tool2->setFixedSize(cur_width, cur_height);
    }

}

void FloatingSettings::setHide(){
    current_page = -1;
    if(tool2 != nullptr) {
        tool2->hide();
        return;
    }
    hide();
}

void FloatingSettings::setPage(int num){
    if(num_of_item < num){
        return;
    }
    printf("%d %d\n", current_page, num);
    if(current_page == num) {
        setHide();
        return;
    }
    current_page = num;
    for(int i=0;i<num_of_item;i++){
        settingsPages.getPage(i)->hide();
    }
    reload();
    if(tool2 != nullptr) {
        tool2->show();
        return;
    }
    show();
}
