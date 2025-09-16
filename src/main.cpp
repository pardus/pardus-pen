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

bool is_wayland;
extern void mainWindowInit();
int history;

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

    is_wayland = (getenv("WAYLAND_DISPLAY") != NULL);
    // gnome wayland fullscreen compositor is buggy.
    // Force prefer Xwayland for fix this issue.
    bool force_xwayland = false;
    if(getenv("XDG_CURRENT_DESKTOP")){
        force_xwayland = strncmp(getenv("XDG_CURRENT_DESKTOP"), "gnome", 5) || \
            strncmp(getenv("XDG_CURRENT_DESKTOP"), "GNOME", 5);
    }
    // Force use X11 or Xwayland
    if(get_bool("xwayland") || force_xwayland){
        setenv("QT_QPA_PLATFORM", "xcb;wayland",1);
        is_wayland = false;
    }
    //Force ignore system dpi
    setenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0", 1);
    setenv("QT_QT_ENABLE_HIGHDPI_SCALING", "0", 1);
    setenv("QT_SCALE_FACTOR", "1", 1);
    //unset qt style override, there's a bug with input when using qt6 build and kvantum
    setenv("QT_STYLE_OVERRIDE", "", 1);

    // history size
    history = get_int("history");
    if(history < 0){
        history = 50;
    }

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
    QCoreApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);

    QCoreApplication::setApplicationName(_("Pardus Pen"));
    QCoreApplication::setOrganizationName("Pardus");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    #ifdef QT5
    (void)qtTranslator.load("qt_" + QLocale::system().name(),
        QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    #else
    (void)qtTranslator.load("qt_" + QLocale::system().name(),
        QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    #endif
    app.installTranslator(&qtTranslator);

    mainWindowInit();

#ifdef LIBARCHIVE
    if (argc > 1) {
        openFile( QString(argv[1]));
    }
#endif
    return app.exec();
}
