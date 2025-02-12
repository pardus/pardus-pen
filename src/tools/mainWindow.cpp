#include "../tools.h"

DrawingWidget *drawing;
Background *background;
FloatingWidget *floatingWidget;
FloatingSettings *floatingSettings;
WhiteBoard *board;
QMainWindow* tool;
QMainWindow* tool2;


QSlider *scrollHSlider;
QSlider *scrollVSlider;
QWidget *mainWidget;

extern int new_x;
extern int new_y;

float scale = 1.0;
extern void setPen(int mode);
extern QColor colors[];

class MainWindow : public QMainWindow {

public:
    QScreen *screen;
    MainWindow() {
        screen = QGuiApplication::primaryScreen();
        scale = screen->size().height() / 1080.0;
        // set attributes
        setAttribute(Qt::WA_TranslucentBackground, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_AcceptTouchEvents, true);

        mainWidget = new QWidget(this);
        mainWidget->setAttribute(Qt::WA_AcceptTouchEvents, true);

        board = new WhiteBoard(mainWidget);
        board->setType(get_int((char*)"page"));
        board->setOverlayType(get_int((char*)"page-overlay"));
        
        // scrolls
        scrollHSlider = new QSlider(Qt::Horizontal, this);
        scrollVSlider = new QSlider(Qt::Vertical, this);
        QObject::connect(scrollHSlider, &QSlider::valueChanged, [=](int value) {
            mainWidget->move(-1*value, mainWidget->y());
        });

        QObject::connect(scrollVSlider, &QSlider::valueChanged, [=](int value) {
            mainWidget->move(mainWidget->x(), value - scrollVSlider->maximum());
        });
        scrollVSlider->setValue(0);
        scrollHSlider->setValue(0);

        scrollVSlider->setVisible(false);
        scrollHSlider->setVisible(false);

    }


    void keyPressEvent(QKeyEvent *event) {
		// https://doc.qt.io/qt-6/qt.html#Key-enum
		// color switch
		bool update = false;
		if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_7){
		    drawing->penColor = colors[20 + event->key() - Qt::Key_1];
		    update = true;
		} else if (event->key() == Qt::Key_8){
		    drawing->penColor = colors[0];
		    update = true;
		} else if (event->key() == Qt::Key_9){
		    drawing->penColor = colors[5];
		    update = true;
		} else {
		    do_shortcut(event->key(), event->modifiers());
		}
		if(update){
		    penStyleEvent();
		    penSizeEvent();
		    backgroundStyleEvent();
		}
}

#define SCROLLSIZE 22*scale

protected:
     void closeEvent(QCloseEvent *event) override {
        puts("Close event");
        event->ignore();
     }
     void resizeEvent(QResizeEvent *event) override {
        screen = QGuiApplication::primaryScreen();
        mainWidget->setFixedSize(screen->size().width(), screen->size().height());
        drawing->setFixedSize(screen->size().width(), screen->size().height());
        background->setFixedSize(screen->size().width(), screen->size().height());
        board->setFixedSize(screen->size().width(), screen->size().height());

        scrollHSlider->setFixedSize(event->size().width() - SCROLLSIZE*2, SCROLLSIZE);
        scrollHSlider->move(SCROLLSIZE, event->size().height() - SCROLLSIZE);
        scrollHSlider->setRange(0, screen->size().width() - event->size().width() );

        scrollVSlider->setFixedSize(SCROLLSIZE, event->size().height() - SCROLLSIZE*2);
        scrollVSlider->move(event->size().width() - SCROLLSIZE, SCROLLSIZE);
        scrollVSlider->setRange(0, screen->size().height() - event->size().height() );

        printf("%d %d\n",event->size().width(), event->size().height());
        new_x = get_int((char*)"cur-x");
        new_y = get_int((char*)"cur-y");
        // tool is not set under wayland
        if(floatingWidget != nullptr) {
            if(tool != nullptr){
                tool->resize(floatingWidget->geometry().width(), floatingWidget->geometry().height());
            }
            if(tool2 != nullptr){
                tool2->resize(floatingSettings->geometry().width(), floatingSettings->geometry().height());
            }
            drawing->update();
        }
        // Call the base class implementation
        QWidget::resizeEvent(event);
        floatingWidget->moveAction();
    }
    void changeEvent(QEvent *event) override {
        // Call the base class implementation
        QMainWindow::changeEvent(event);
        if(tool != nullptr){
            if (event->type() == QEvent::WindowStateChange) {
                if (isMinimized()) {
                    tool->hide();
                } else {
                    tool->show();
                }
            }
        }
        floatingSettings->setHide();
        drawing->update();
    }
};
static MainWindow *mainWindow;
static bool isFullScreen = true;
static bool hideState = true;

void setupTools(){
#ifndef ETAP19
    // detect x11
    if(!getenv("WAYLAND_DISPLAY")){
        // main toolbar
        tool = new QMainWindow();
        tool->setWindowFlags(Qt::WindowStaysOnTopHint
                              | Qt::Tool
                              | Qt::X11BypassWindowManagerHint
                              | Qt::WindowSystemMenuHint
                              | Qt::WindowStaysOnTopHint
                              | Qt::FramelessWindowHint);
        tool->setAttribute(Qt::WA_TranslucentBackground, true);
        tool->setAttribute(Qt::WA_NoSystemBackground, true);
        tool->setStyleSheet(
            "background: none;"
            "color: black;"
            "font-size: "+QString::number(18*scale)+"px;"
        );

        // second toolbar
        tool2 = new QMainWindow();
        tool2->setWindowFlags(Qt::WindowStaysOnTopHint
                              | Qt::Tool
                              | Qt::X11BypassWindowManagerHint
                              | Qt::WindowStaysOnTopHint
                              | Qt::WindowSystemMenuHint
                              | Qt::FramelessWindowHint);
        tool2->setAttribute(Qt::WA_TranslucentBackground, true);
        tool2->setAttribute(Qt::WA_NoSystemBackground, true);
        tool2->setStyleSheet(
            "background: none;"
            "color: black;"
            "font-size: "+QString::number(18*scale)+"px;"
        );
        floatingSettings = new FloatingSettings(tool2);
        floatingWidget = new FloatingWidget(tool);
        tool->setCentralWidget(floatingWidget);
        tool2->setCentralWidget(floatingSettings);

        tool->show();
        tool2->hide();
    } else {
#endif
        tool = nullptr;
        floatingSettings = new FloatingSettings(mainWindow);
        floatingWidget = new FloatingWidget(mainWindow);
#ifndef ETAP19
    }
#endif
    floatingWidget->setSettings(floatingSettings);
    floatingSettings->setHide();

    minify = create_button(":images/screen.svg", [=](){
            mainWindow->showMinimized();
    });
    set_shortcut(minify, Qt::Key_D, Qt::MetaModifier);

    QScreen *screen = QGuiApplication::primaryScreen();
    fullscreen = create_button(":images/fullscreen-exit.svg", [=](){
        mainWidget->move(0,0);
        if ((tool != nullptr) && (tool2 != nullptr)){
            tool->hide();
            tool2->hide();
            tool->show();
            tool2->show();
        }
        mainWindow->hide();
        mainWindow->showNormal();
        if(isFullScreen){
            set_icon(":images/fullscreen.svg", fullscreen);
            mainWindow->resize(screen->size().width() * 0.8, screen->size().height() * 0.8);
        } else {
            set_icon(":images/fullscreen-exit.svg", fullscreen);
            mainWindow->resize(screen->size().width(), screen->size().height());
            mainWindow->showFullScreen();
        }
        scrollVSlider->setVisible(isFullScreen);
        scrollHSlider->setVisible(isFullScreen);
        isFullScreen = !isFullScreen;
        minify->setEnabled(isFullScreen);
    });

    set_shortcut(fullscreen, Qt::Key_F11, 0);

    rotate = create_button(":images/rotate.svg", [=](){
        floatingWidget->setVertical(!floatingWidget->is_vertical);
        floatingSettings->setHide();
    });

    // non-gui button for hide / show floatingWidget
    QPushButton* hideUi = create_button("", [=](){
        if(hideState){
            floatingWidget->hide();
            floatingSettings->setHide();
        } else {
            floatingWidget->show();
        }
        hideState = ! hideState;
    });
    set_shortcut(hideUi, Qt::Key_F1, Qt::AltModifier);
}

void setHideMainWindow(bool status){
    if(status){
        mainWindow->hide();
    } else{
        if(isFullScreen){
            mainWindow->showFullScreen();
        } else {
            mainWindow->showNormal();
        }
    }
}

void mainWindowInit(){
    mainWindow = new MainWindow();
    background = new Background(mainWidget);
    drawing = new DrawingWidget(mainWidget);
    ov = new OverView();
    setupTools();
    setupPenType();
    setupBackground();
    setupScreenShot();
    setupSaveLoad();

    mainWindow->setWindowTitle(QString(_("Pardus Pen")));
    mainWindow->setWindowIcon(QIcon(":tr.org.pardus.pen.svg"));
    floatingWidget->setMainWindow(mainWindow);
    setupWidgets();
    mainWindow->showFullScreen();
    QScreen *screen = QGuiApplication::primaryScreen();
    mainWindow->resize(screen->size().width(), screen->size().height());
}
