#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>
#include <QProcess>
#include <QTranslator>

#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#include "tools.h"

extern void mainWindowInit();


int main(int argc, char *argv[]) {
    settings_init();
#ifdef ETAP19
    QStringList args1;
    QProcess p1;
    args1 << "-e" << "eta-disable-gestures@pardus.org.tr";
    p1.execute("gnome-shell-extension-tool", args1);

    QStringList args2;
    QProcess p2;
    args2 << "set" << "org.gnome.mutter" << "overlay-key" << "''";
    p2.execute("gsettings", args2);
#endif

    // Force use X11 or Xwayland
    if(get_bool((char*)"xwayland")){
        setenv("QT_QPA_PLATFORM", "xcb;wayland",1);
    }
    //Force ignore system dpi
    setenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0", 1);
    setenv("QT_SCALE_FACTOR", "1", 1);


    // translation part
    const char *systemLanguage = getenv("LANG");
    if (systemLanguage != nullptr) {
        bindtextdomain("pardus-pen", "/usr/share/locale");
        setlocale(LC_ALL, systemLanguage);
        textdomain("pardus-pen");
    } else {
        fprintf(stderr, "WARNING: LANG environment variable not set.\n");
    }

    #ifdef QT5
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    QCoreApplication::setApplicationName(_("Pardus Pen"));
    QCoreApplication::setOrganizationName("Pardus");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    (void)qtTranslator.load("qt_" + QLocale::system().name(),
        QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    mainWindowInit();

#ifdef LIBARCHIVE
    if (argc > 1) {
        pthread_t ptid;
        archive_target = QString(argv[1]);
        pthread_create(&ptid, NULL, &load_archive, NULL);
    }
#endif
    return app.exec();
}
