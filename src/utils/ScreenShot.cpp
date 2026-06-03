#if screenshot
#include <string.h>
#include <stdlib.h>
#include <libintl.h>

#include <iostream>

#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>

#include <constants.h>

#include <utils/ScreenShot.h>
#include <utils/misc.h>
#include <widgets/DrawingWidget.h>
#include <widgets/WhiteBoard.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int run_cmd(const char* args[]){
    pid_t pid = fork();
    int status = 0;
    if (pid == 0){
        execv(args[0], (char *const *)args);
        exit(1);
    } else {
        waitpid(pid, &status, 0);
    }
    return status;
}

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
                char* spectacle = which((char*)"spectacle");
                std::string imgnamestr = imgname.toStdString();
                if(strlen(spectacle) > 0){
                    const char* cmd[] = {
                        "/usr/bin/env",
                        "QT_QPA_PLATFORM='wayland'",
                        spectacle,
                        "-fbnmo",
                        imgnamestr.c_str(),
                        NULL
                    };
                    status = run_cmd(cmd);
                }
                free(spectacle);
                char* grim = which((char*)"grim");
                if(status > 1 || strlen(grim) > 0){
                    const char* cmd[] = {
                        grim,
                        "-t",
                        "png",
                        imgnamestr.c_str(),
                        NULL
                    };
                    status = run_cmd(cmd);
                }
                free(grim);
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
