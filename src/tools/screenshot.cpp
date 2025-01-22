#include "../tools.h"
#include <QTimer>
QPushButton *ssButton;

void setupScreenShot(){
    #ifdef screenshot
    QTimer *ssTimer = new QTimer();
    QObject::connect(ssTimer, &QTimer::timeout, [=](){
        takeScreenshot();
        floatingWidget->show();
        ssTimer->stop();

    });
    ssButton = create_button(":images/screenshot.svg", [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        ssTimer->start(500);

    });
    set_shortcut(ssButton, Qt::Key_F2, 0);
    #endif
}
