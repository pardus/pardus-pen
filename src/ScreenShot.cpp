#include <string.h>
#include <stdlib.h>

#include <QString>
#include <QStandardPaths>
#include <QStandardPaths>
#include <QDateTime>
#include <QMessageBox>

#include "ScreenShot.h"

extern "C" {
#include "which.h"
}

void takeScreenshot(){
    QString pics = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDateTime time = QDateTime::currentDateTime();

    QString imgname = pics + "/" + time.toString("yyyy-MM-dd_hh-mm-ss") + ".png";
    int status = 1;
    if(strcmp("",which((char*)"spectatle")) != 0){
        status = system(("spectacle -fbnmo "+imgname.toStdString()).c_str());
    } else if(strcmp("",which((char*)"scrot")) != 0){
        status = system(("scrot "+imgname.toStdString()).c_str());
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