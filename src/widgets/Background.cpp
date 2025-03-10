#include <QtWidgets>
#include <QPainter>
#include "Background.h"

#include "../tools.h"

#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#define _(String) gettext(String)

Background::Background(QWidget *parent) : QWidget(parent) {
    setStyleSheet("background: none");
    mainWindow = (QMainWindow*)parent;
    image = QImage(size(), QImage::Format_ARGB32);
    image.fill(QColor("transparent"));
    show();
}

void Background::applyImage(QImage img){
    painter.begin(&image);
    painter.setPen(Qt::NoPen);
    painter.drawImage(QPoint(0,0), img);
    painter.end();
}
void Background::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawImage(QPoint(0,0), image);
    painter.end();
}

