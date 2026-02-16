#include "../tools.h"

#include <QTimer>

#ifdef screenshot

static void setCropScreenShot(QPixmap pix){
    if(pix.size().width() > 30 && pix.size().height() > 30){
        drawing->goNextPage();
        drawing->setOverlay(pix.toImage(), drawing->getPageNum());
        board->setType(WHITE);
        board->setOverlayType(CUSTOM);
        board->rotates[drawing->getPageNum()] = 0;
        board->ratios[drawing->getPageNum()] = 100;
        board->updateTransform();
    }
    setHideMainWindow(false);
    floatingWidget->show();

}

ScreenshotWidget* ssWidget;
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

    ssWidget = new ScreenshotWidget();
    ssWidget->crop_signal = setCropScreenShot;

    toolButtons[SCREENSHOT_CROP] = create_button(SCREENSHOT_CROP, [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        ssWidget->showFullScreen();
        if(tool3){
            tool3->hide();
        }
    });
    set_shortcut(toolButtons[SCREENSHOT_CROP], Qt::Key_F2, Qt::ControlModifier);
    #endif
}
