#include <string.h>
#include <stdlib.h>

#include <QString>
#include <QStandardPaths>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>

#include <iostream>

#include "ScreenShot.h"

extern "C" {
#include "which.h"
}

void takeScreenshot(){
    QString pics = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDateTime time = QDateTime::currentDateTime();

    QString imgname = pics + "/" + time.toString("yyyy-MM-dd_hh-mm-ss") + ".png";
    int status = 1;
    std::string spectacle(which((char*)"spectacle"));
    std::string gnome_screenshot(which((char*)"gnome-screenshot"));
    std::string scrot(which((char*)"scrot"));
    if(strlen(spectacle.c_str()) != 0){
        status = system(("QT_QPA_BACKEND='' "+spectacle+" -fbnmo "+imgname.toStdString()).c_str());
    } else if(strlen(gnome_screenshot.c_str()) > 0){
        status = system((gnome_screenshot+" -f "+imgname.toStdString()).c_str());
    } else if(strlen(scrot.c_str()) > 0){
        status = system((scrot+" "+imgname.toStdString()).c_str());
    }
    QMessageBox messageBox;
    Qt::WindowFlags flags =  Qt::Dialog | Qt::X11BypassWindowManagerHint;
    messageBox.setWindowFlags(flags);
    messageBox.setText("Info");
    std::string msg;
    if (status == 0){
        msg = "Screenshot saved:" + imgname.toStdString() + "\n";
    } else {
        msg = "Failed To save:" + imgname.toStdString() + "\n";
    }
    messageBox.setInformativeText(msg.c_str());
    messageBox.setIcon(QMessageBox::Information);
    messageBox.exec();
}
