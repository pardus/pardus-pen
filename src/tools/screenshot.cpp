#include "../tools.h"

#include <QTimer>

#ifdef screenshot

static void setCropScreenShot(QPixmap pix){
    drawing->setOverlay(pix.toImage(), drawing->getPageNum());
    board->setType(WHITE);
    board->setOverlayType(CUSTOM);
    board->rotates[drawing->getPageNum()] = 0;
    board->ratios[drawing->getPageNum()] = pix.width() * 100 / board->geometry().width();
    board->updateTransform();
    setHideMainWindow(false);

    printf("%s\n", "crop done");
}
#endif

void setupScreenShot(){
    #ifdef screenshot
    QTimer *ssTimer = new QTimer();
    QObject::connect(ssTimer, &QTimer::timeout, [=](){
        takeScreenshot();
        floatingWidget->show();
        ssTimer->stop();

    });
    toolButtons[SCREENSHOT] = create_button(SCREENSHOT, [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        ssTimer->start(500);

    });
    set_shortcut(toolButtons[SCREENSHOT], Qt::Key_F2, 0);

    ScreenshotWidget* widget = new ScreenshotWidget();
    widget->crop_signal = setCropScreenShot;
    QTimer *sscTimer = new QTimer();
    QObject::connect(sscTimer, &QTimer::timeout, [=](){
        sscTimer->stop();
        widget->showFullScreen();
        floatingWidget->show();

    });
    toolButtons[SCREENSHOT_CROP] = create_button(SCREENSHOT_CROP, [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        sscTimer->start(500);
    });
    set_shortcut(toolButtons[SCREENSHOT_CROP], Qt::Key_F2, Qt::ControlModifier);
    #endif
}
