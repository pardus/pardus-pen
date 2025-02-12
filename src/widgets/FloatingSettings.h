#ifndef FLOATINGSETTINGS_H
#define FLOATINGSETTINGS_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QScreen>
#include <QApplication>

class QLabel;

class FloatingSettings : public QWidget {
public:
    int cur_width = 0;
    int cur_height = 0;
    FloatingSettings(QWidget *parent = nullptr);
    void addPage(QWidget *widget);
    void setPage(int num);
    void reload();
    void setHide();
    int current_page = -1;
private:
    int num_of_item = 0;
    QPoint dragPosition;
    QVBoxLayout *layout;
};

#endif // FLOATINGSETTINGS_H
