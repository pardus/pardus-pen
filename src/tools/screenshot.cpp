#include "../tools.h"
#include <QTimer>

void setupScreenShot(){
    #ifdef screenshot
    QTimer *ssTimer = new QTimer();
    QObject::connect(ssTimer, &QTimer::timeout, [=](){
        takeScreenshot();
        floatingWidget->show();
        ssTimer->stop();

    });
    toolButtons[SCREENSHOT] = create_button(":images/screenshot.svg", [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        ssTimer->start(500);

    });
    set_shortcut(toolButtons[SCREENSHOT], Qt::Key_F2, 0);
    #endif
}
