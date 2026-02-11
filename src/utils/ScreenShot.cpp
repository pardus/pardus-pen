#if screenshot
#include <string.h>
#include <stdlib.h>
#include <libintl.h>

#include <iostream>

#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>

#include "../utils/ScreenShot.h"
#include "../widgets/DrawingWidget.h"

#include "../tools.h"

extern DrawingWidget *drawing;


#define _(String) gettext(String)


void takeScreenshot(){
    QString pics = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDateTime time = QDateTime::currentDateTime();
    QString imgname = pics + "/" + time.toString("yyyy-MM-dd_hh-mm-ss") + ".png";
    int status = 1;
    if (getPen() == SELECTION) {
        drawing->cropWidget->image.save(imgname);
        status = 0;
    } else if(getPen() != SELECTION) {
        // detect X11
        if (!getenv("WAYLAND_DISPLAY")){
            QPixmap pixmap;
            if(board->getType() == TRANSPARENT){
                QScreen *screen = QGuiApplication::primaryScreen();
                pixmap = screen->grabWindow(0);
            } else {
                pixmap = mainWidget->grab();
            }
            QFile file(imgname);
            status = 1;
            if(file.open(QIODevice::WriteOnly)){
                pixmap.save(&file, "PNG");
                status = 0;
            }
        } else {
            #ifdef DBUS
            char* ss = screenshot_xdg_portal();
            if(ss){
                // ss = "file:///home/pingu/Pictures/Screenshot.png"
                if(strncmp(ss, "file://", 6) == 0){
                    rename(ss+6, imgname.toStdString().c_str());
                    status = 0;
                }
                free(ss);
            } else {
            #endif
                std::string spectacle(which((char*)"spectacle"));
                std::string grim(which((char*)"grim"));
                if(strlen(spectacle.c_str()) != 0){
                    status = system(("QT_QPA_PLATFORM='wayland' "+spectacle+" -fbnmo "+imgname.toStdString()).c_str());
                } else if(strlen(grim.c_str()) != 0){
                    status = system((grim+" -t png "+imgname.toStdString()).c_str());
                }
            #ifdef DBUS
            }
            #endif
        }
    }
    // show message
    QMessageBox messageBox;
    Qt::WindowFlags flags =  Qt::Dialog | Qt::X11BypassWindowManagerHint;
    messageBox.setWindowFlags(flags);
    
    if (status == 0){
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(_("Screenshot Saved"));
        messageBox.setInformativeText(_("Screenshot saved to:") + imgname);

        QPushButton *openFileButton = messageBox.addButton(_("Open File"), QMessageBox::ActionRole);
        QPushButton *openFolderButton = messageBox.addButton(_("Open Containing Folder"), QMessageBox::ActionRole);
        messageBox.addButton(QMessageBox::Ok);

        messageBox.exec();

        if (messageBox.clickedButton() == openFileButton) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(imgname));
        } else if (messageBox.clickedButton() == openFolderButton) {
            QFileInfo fileInfo(imgname);
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
        }
        delete openFileButton;
        delete openFolderButton;
    } else {
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(_("Error Saving Screenshot"));
        std::string fail_msg_str = _("Failed to save screenshot to:") + imgname.toStdString();
        messageBox.setInformativeText(QString::fromStdString(fail_msg_str));
        messageBox.addButton(QMessageBox::Ok);
        messageBox.exec();
    }
}

#endif
