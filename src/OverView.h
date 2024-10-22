#include <QPoint>
#include <QPainter>
#include <QWidget>


class OverView : public QWidget {
public:
    float penSize;
    int penType;
    QColor color;
    QColor background;
    void updateImage();
protected:
    void paintEvent(QPaintEvent *event) override;
    QPainter painter;
};
