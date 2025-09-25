#include "../tools.h"

DrawingWidget *drawing;
Background *background;
FloatingWidget *floatingWidget;
FloatingWidget *desktopWidget;
FloatingSettings *floatingSettings;
WhiteBoard *board;

QMainWindow* tool;
QMainWindow* tool2;
QMainWindow* tool3;

QSlider *scrollHSlider;
QSlider *scrollVSlider;
QWidget *mainWidget;

float scale = 1.0;
extern void setPen(int mode);
extern QColor colors[];

class MainWindow : public QMainWindow {

#define SCROLLSIZE 22*scale

public:
    QScreen *screen;
    MainWindow() {
        screen = QGuiApplication::primaryScreen();
        scale = screen->size().height() / 1080.0;
        if(scale < 1){
            scale = 1.0;
        }
        // set attributes
        setAttribute(Qt::WA_TranslucentBackground, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_AcceptTouchEvents, true);

        mainWidget = new QWidget(this);
        mainWidget->setAttribute(Qt::WA_AcceptTouchEvents, true);

        board = new WhiteBoard(mainWidget);
        board->setType(get_id_by_overlay(get_string("page")));
        board->setOverlayType(get_id_by_overlay(get_string("page-overlay")));

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

    void showCloseDialog(){
        QMessageBox msgBox;
        msgBox.setWindowTitle(_("Pardus Pen"));
        msgBox.setText(_("Exit"));
        msgBox.setInformativeText(_("Are you sure you want to exit?"));
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.addButton(QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        if(msgBox.exec() == QMessageBox::Yes){
            exit(0);
        }
    }

protected:

    void keyPressEvent(QKeyEvent *event) override {
        // https://doc.qt.io/qt-6/qt.html#Key-enum
        // color switch
        bool update = false;
        if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_7){
            drawing->penColor = colors[13 + event->key() - Qt::Key_1];
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
            updateGui();
        }
    }


    void closeEvent(QCloseEvent *event) override {
        puts("Close event");
        showCloseDialog();
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

        debug("width:%d height:%d \n",event->size().width(), event->size().height());
        floatingWidget->new_x = get_int("cur-x");
        floatingWidget->new_y = get_int("cur-y");
        // tool is not set under wayland
        if(floatingWidget != nullptr) {
            if(!is_wayland){
                tool->resize(floatingWidget->geometry().width(), floatingWidget->geometry().height());
            }
            if(!is_wayland){
                tool2->resize(floatingSettings->geometry().width(), floatingSettings->geometry().height());
            }
            drawing->update();
        }
        floatingWidget->moveAction();
        // Call the base class implementation
        QWidget::resizeEvent(event);
    }
    void changeEvent(QEvent *event) override {
        // Call the base class implementation
        QMainWindow::changeEvent(event);
        if (event->type() == QEvent::WindowStateChange) {
            if (isMinimized()) {
                enable_erc();
            } else {
                disable_erc();
            }
        }
        if(!is_wayland){
            if (event->type() == QEvent::WindowStateChange) {
                tool2->hide();
                if (isMinimized()) {
                    tool->hide();
                    tool3->show();
                } else {
                    tool->show();
                    tool3->hide();
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
    if(!is_wayland){
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
        );

        // third toolbar
        tool3 = new QMainWindow();
        tool3->setWindowFlags(Qt::WindowStaysOnTopHint
                              | Qt::Tool
                              | Qt::X11BypassWindowManagerHint
                              | Qt::WindowSystemMenuHint
                              | Qt::WindowStaysOnTopHint
                              | Qt::FramelessWindowHint);
        tool3->setAttribute(Qt::WA_TranslucentBackground, true);
        tool3->setAttribute(Qt::WA_NoSystemBackground, true);
        tool3->setStyleSheet(
            "background: none;"
        );

        floatingSettings = new FloatingSettings(tool2);
        floatingWidget = new FloatingWidget(tool);
        desktopWidget = new FloatingWidget(tool3);
        tool->setCentralWidget(floatingWidget);
        tool2->setCentralWidget(floatingSettings);
        tool3->setCentralWidget(desktopWidget);

        tool->show();
        tool3->show();
        tool2->hide();
    } else {
#endif
        tool = nullptr;
        tool2 = nullptr;
        tool3 = nullptr;
        floatingSettings = new FloatingSettings(mainWindow);
        floatingWidget = new FloatingWidget(mainWindow);
        desktopWidget = new FloatingWidget(mainWindow);
#ifndef ETAP19
    }
#endif
    floatingWidget->setSettings(floatingSettings);
    desktopWidget->setSettings(floatingSettings);
    floatingSettings->setHide();

    toolButtons[MINIFY] = create_button(MINIFY, [=](){
        if(!mainWindow->isMinimized()){
            mainWindow->showMinimized();
        } else {
             mainWindow->showFullScreen();
        }
    });
    set_shortcut(toolButtons[MINIFY], Qt::Key_D, Qt::MetaModifier);

    toolButtons[UNMINIFY] = create_button(ICON, [=](){
         mainWindow->showFullScreen();
    });

    QScreen *screen = QGuiApplication::primaryScreen();
    toolButtons[FULLSCREEN] = create_button(FULLSCREEN_EXIT, [=](){
        mainWidget->move(0,0);
        if (!is_wayland){
            tool->hide();
            tool2->hide();
            tool->show();
            tool2->show();
        }
        mainWindow->hide();
        if(isFullScreen){
            set_icon(get_icon_by_id(FULLSCREEN), toolButtons[FULLSCREEN]);
            mainWindow->resize(screen->size().width() * 0.8, screen->size().height() * 0.8);
            mainWindow->showNormal();
        } else {
            set_icon(get_icon_by_id(FULLSCREEN_EXIT), toolButtons[FULLSCREEN]);
            mainWindow->resize(screen->size().width(), screen->size().height());
            mainWindow->showFullScreen();
        }
        scrollVSlider->setVisible(isFullScreen);
        scrollHSlider->setVisible(isFullScreen);
        isFullScreen = !isFullScreen;
        toolButtons[MINIFY]->setEnabled(isFullScreen);
    });

    set_shortcut(toolButtons[FULLSCREEN], Qt::Key_F11, 0);

    toolButtons[ROTATE] = create_button(ROTATE, [=](){
        floatingWidget->is_vertical = !floatingWidget->is_vertical;
        floatingWidget->setVertical(floatingWidget->is_vertical);
        floatingSettings->setHide();
    });


    // non-gui button for hide / show floatingWidget
    toolButtons[HIDEUI] = create_button(0, [=](){
        if(hideState){
            floatingWidget->hide();
            floatingSettings->setHide();
        } else {
            floatingWidget->show();
        }
        hideState = ! hideState;
    });
    set_shortcut(toolButtons[HIDEUI], Qt::Key_F1, Qt::AltModifier);
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
    setupBackground();
    setupPenType();
    setupScreenShot();
    setupSaveLoad();

    mainWindow->setWindowTitle(QString(_("Pardus Pen")));
    mainWindow->setWindowIcon(QIcon(":tr.org.pardus.pen.svg"));
    floatingWidget->setMainWindow(mainWindow);
    desktopWidget->setMainWindow(mainWindow);
    setupWidgets();
    mainWindow->showFullScreen();
    QScreen *screen = QGuiApplication::primaryScreen();
    mainWindow->resize(screen->size().width(), screen->size().height());


    setPen(PEN);
}
