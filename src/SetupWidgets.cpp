#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>
#include <QMessageBox>
#include <QSlider>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "FloatingSettings.h"
#include "WhiteBoard.h"
#include "Button.h"
#include "ScreenShot.h"
#include "OverView.h"


extern "C" {
#include "settings.h"
}


#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#define _(String) gettext(String)

#define padding 8

extern DrawingWidget *window;
extern FloatingWidget *floatingWidget;
extern FloatingSettings *floatingSettings;
extern WhiteBoard *board;
extern QMainWindow* mainWindow;

QPushButton *penButton;
QPushButton *typeButton;
QPushButton *markerButton;
QPushButton *eraserButton;
QPushButton *splineButton;
QPushButton *lineButton;
QPushButton *circleButton;

QPushButton *backgroundButton;

QPushButton* transparentButton;
QPushButton* blackButton;
QPushButton* whiteButton;
QPushButton* colorpicker;

QPushButton* overlayLines;
QPushButton* overlaySquares;
QPushButton* overlayNone;


QPushButton *backButton;
QPushButton *nextButton;

QPushButton *previousPage;
QPushButton *nextPage;

OverView *ov;

extern int screenWidth;
extern int screenHeight;

bool sliderLock = false;

QWidget *penSettings;
QWidget *colorDialog;
QWidget *typeDialog;
QSlider *thicknessSlider;
QLabel *thicknessLabel;
QLabel *colorLabel;

QString penText = "";

#define butsize screenHeight/23

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static void penStyleEvent(){
    penButton->setStyleSheet(QString("background-color: none;"));
    markerButton->setStyleSheet(QString("background-color: none;"));
    eraserButton->setStyleSheet(QString("background-color: none;"));
    lineButton->setStyleSheet(QString("background-color: none;"));
    splineButton->setStyleSheet(QString("background-color: none;"));
    circleButton->setStyleSheet(QString("background-color: none;"));
    switch(window->penType){
        case PEN:
            penButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        case MARKER:
            markerButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        default:
            eraserButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
    }
    switch(window->penStyle){
        case LINE:
            lineButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        case CIRCLE:
            circleButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        default:
            splineButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
    }
    ov->penSize = window->penSize[window->penType];
    ov->color = window->penColor;
    ov->updateImage();
}


static void setCursor(const char* name){
    QIcon icon = QIcon(name);
    QPixmap pixmap = icon.pixmap(
        icon.actualSize(
            QSize(window->penSize[window->penType],
            window->penSize[window->penType])
        )
   );
   QCursor cur(pixmap);
   window->setCursor(cur);
   floatingSettings->setCursor(cur);
}

static void penSizeEvent(){
    int value = window->penSize[window->penType];
    switch(window->penType){
        case PEN:
            penText = _("Pen");
            set_int((char*)"pen-size",value);
            break;
        case MARKER:
            penText = _("Marker");
            set_int((char*)"marker-size",value);
            break;
        case ERASER:
            penText = _("Eraser");
            set_int((char*)"eraser-size",value);
            break;
    }
    thicknessLabel->setText(QString(penText)+QString(_(" Size: "))+QString::number(value));
    colorLabel->setText(QString(penText)+QString(_(" Color:")));


    if(window->penType == ERASER) {
        penSettings->setFixedSize(
            colorDialog->size().width() + padding*2,
            padding*2
             + thicknessLabel->size().height()
             + thicknessSlider->size().height()
        );
        colorDialog->hide();
        ov->hide();
        colorLabel->hide();
        setCursor(":images/cursor.svg");
    } else {
        penSettings->setFixedSize(
            colorDialog->size().width() + padding*2,
            padding*2
             + ov->size().height()
             + thicknessLabel->size().height()
             + thicknessSlider->size().height()
             + colorDialog->size().height()
             + colorLabel->size().height()
        );
        colorDialog->show();
        colorLabel->show();
        ov->show();
        window->unsetCursor();
        floatingSettings->unsetCursor();
    }
    ov->penSize = value;
    ov->color = window->penColor;
    ov->updateImage();
    floatingSettings->reload();
}

void updateGoBackButtons(){
    if(window->isBackAvailable()){
        set_icon(":images/go-back.svg", backButton);
    } else{
        set_icon(":images/go-back-disabled.svg", backButton);
    }
    if(window->isNextAvailable()){
        set_icon(":images/go-next.svg", nextButton);
    } else{
        set_icon(":images/go-next-disabled.svg", nextButton);
    }
    if(window->getPageNum() == 0){
        set_icon(":images/go-page-previous-disabled.svg", previousPage);
    } else {
        set_icon(":images/go-page-previous.svg", previousPage);
    }
}


static void backgroundStyleEvent(){
    transparentButton->setStyleSheet(QString("background-color: none;"));
    blackButton->setStyleSheet(QString("background-color: none;"));
    whiteButton->setStyleSheet(QString("background-color: none;"));
    overlayLines->setStyleSheet(QString("background-color: none;"));
    overlaySquares->setStyleSheet(QString("background-color: none;"));
    overlayNone->setStyleSheet(QString("background-color: none;"));
    switch(board->getType()){
        case BLACK:
            set_icon(":images/paper-black.svg",backgroundButton);
            blackButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            ov->background = Qt::black;
            break;
        case WHITE:
            whiteButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            set_icon(":images/paper-white.svg",backgroundButton);
            ov->background = Qt::white;
            break;
        default:
            transparentButton->setStyleSheet("background-color:"+window->penColor.name()+";");
            set_icon(":images/paper-transparent.svg",backgroundButton);
            ov->background = Qt::transparent;
            break;
    }
    switch(board->getOverlayType()){
        case LINES:
            overlayLines->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        case SQUARES:
            overlaySquares->setStyleSheet("background-color:"+window->penColor.name()+";");
            break;
        default:
            overlayNone->setStyleSheet("background-color:"+window->penColor.name()+";");
    }
    ov->updateImage();
}


static void setupMove(){
    QLabel *move = new QLabel("");
    QIcon icon = QIcon(":images/move-icon.svg");
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(screenHeight/23, screenHeight/23)));
    move->setPixmap(pixmap);
    move->setStyleSheet(QString("background-color: none;"));
    move->setFixedSize(butsize, butsize);
    floatingWidget->setWidget(move);
}


static void setupPenSize(){

    QPushButton *penSettingsButton = create_button(":images/pen-settings.svg",  [=](){
        floatingSettings->setPage(1);
        floatingWidget->setFloatingOffset(5);
    });
    penSettingsButton->setStyleSheet(QString("background-color: none;"));

    // Thickness settings

    penSettings = new QWidget();
    QVBoxLayout *penSettingsLayout = new QVBoxLayout(penSettings);
    penSettings->setStyleSheet(
        "QWidget {"
            "background-color: none;"
         "}"
         "QSlider::groove:horizontal {"
            "border: 1px solid #bbb;"
            "background: white;"
            "height: "+QString::number(screenHeight / 50)+"px;"
            "border-radius: "+QString::number(screenHeight / 100)+"px;"
        "}"
        "QSlider::handle:horizontal {"
            "background: #fff;"
            "border: 1px solid #777;"
            "width: "+QString::number(screenHeight / 25)+"px;"
            "margin: -"+QString::number(screenHeight / 216)+"px 0;"
            "border-radius: "+QString::number(screenHeight / 100)+"px;"
        "}"
        "QSlider::handle:horizontal:hover {"
            "background: #ccc;"
        "}"
        "QSlider::sub-page:horizontal {"
            "background: #5FAEE3;"
            "border-radius: 5px;"
        "}"
        "QSlider::add-page:horizontal {"
            "background: #FBFBFB;"
            "border-radius:"+QString::number(screenHeight / 200)+"px;"
        "}"
    );

    ov->updateImage();
    penSettingsLayout->addWidget(ov);

    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setRange(1,31);
    thicknessSlider->setSingleStep(1);
    thicknessSlider->setValue(window->penSize[PEN]);


    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penSettingsLayout->setSpacing(0);


    thicknessLabel = new QLabel();
    thicknessLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 
 
    penSettingsLayout->addWidget(thicknessLabel);
    penSettingsLayout->addWidget(thicknessSlider);

    penSettings->show();

    QObject::connect(thicknessSlider, &QSlider::valueChanged, [=](int value) {
        if(!sliderLock){
            window->penSize[window->penType] = value;
        }
        penSizeEvent();
    });

    // Color Settings    
    colorLabel = new QLabel();
    colorLabel->setText(QString(penText)+QString(_(" Color:")));
    colorLabel->setAlignment(Qt::AlignHCenter);

    colorDialog = new QWidget();
    colorDialog->setWindowTitle(_("Color Picker"));


    QGridLayout *gridLayout = new QGridLayout(colorDialog);

    // Define colors
    QColor colors[] = {
        // row 1
        QColor("#000000"),
        QColor("#3c4043"),
        QColor("#5f6368"),
        QColor("#9aa0a6"),
        QColor("#dadce0"),
        QColor("#ffffff"),
        // row 2
        QColor("#f28b82"),
        QColor("#fdd663"),
        QColor("#81c995"),
        QColor("#78d9ec"),
        QColor("#8ab4f8"),
        QColor("#c58af9"),
        QColor("#eec9ae"),
        // row 3
        QColor("#ea4335"),
        QColor("#fbbc04"),
        QColor("#34a853"),
        QColor("#24c1e0"),
        QColor("#4285f4"),
        QColor("#a142f4"),
        QColor("#e2a185"),
        // row 4
        QColor("#c5221f"),
        QColor("#f29900"),
        QColor("#188038"),
        QColor("#12a4af"),
        QColor("#1967d2"),
        QColor("#8430ce"),
        QColor("#885945")
    };


    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(padding);

    colorpicker = create_button(":images/color-picker.svg", [=](){
        QColor newCol = QColorDialog::getColor(window->penColor, mainWindow, _("Select Color"));
        if(! newCol.isValid()){
            return;
        }
        window->penColor = newCol;
        set_string((char*)"color", (char*)window->penColor.name().toStdString().c_str());
        penStyleEvent();
        backgroundStyleEvent();
    });
    colorpicker->setStyleSheet(QString("background: none;"));



    // Create buttons for each color
    int num_of_color = sizeof(colors) / sizeof(QColor);
    int rowsize = 7;
    gridLayout->addWidget(colorpicker, 0, 0, Qt::AlignCenter);
    for (int i = 0; i < num_of_color; i++) {
        QPushButton *button = new QPushButton(colorDialog);
        button->setFixedSize(butsize, butsize);
        button->setStyleSheet(QString(
             "background-color: %1;"
             "border-radius: 12px;"
             "border: 1px solid "+convertColor(colors[i]).name()+";"
        ).arg(colors[i].name()));
        QObject::connect(button, &QPushButton::clicked, [=]() {
            window->penColor = colors[i];
            set_string((char*)"color", (char*)window->penColor.name().toStdString().c_str());
            penStyleEvent();
            backgroundStyleEvent();
        });
        gridLayout->addWidget(button, (i+1) / rowsize, (i+1) % rowsize, Qt::AlignCenter);
    }

   

    colorDialog->setLayout(gridLayout);
    colorDialog->setFixedSize(
        butsize*rowsize + padding*(rowsize),
        butsize*(1+(num_of_color/rowsize))+ padding*((num_of_color / rowsize))
    );

    colorLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    colorLabel->setFixedSize(
       colorDialog->size().width(),
       butsize / 2
    );

    thicknessSlider->setFixedSize(
        colorDialog->size().width(),
        butsize
    );
    thicknessLabel->setFixedSize(
        colorDialog->size().width(),
        colorLabel->size().height()
    );
    
    ov->setFixedSize(
        colorDialog->size().width(),
        butsize*3
    );

    penSettingsLayout->addWidget(colorLabel);
    penSettingsLayout->addWidget(colorDialog);

    floatingSettings->addPage(penSettings);
    floatingWidget->setWidget(penSettingsButton);

    penSizeEvent();

    floatingSettings->reload();
}

static void setupPenType(){

    ov = new OverView();

    penButton = create_button(":images/pen.svg", [=](){
        if(window->penType == PEN){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        window->penType = PEN;
        penStyleEvent();
        thicknessSlider->setRange(1,31);
        thicknessSlider->setValue(window->penSize[PEN]);
        penSizeEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(penButton);

    markerButton = create_button(":images/marker.svg", [=](){
        if(window->penType == MARKER){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        window->penType = MARKER;
        penStyleEvent();
        thicknessSlider->setRange(1,100);
        thicknessSlider->setValue(window->penSize[MARKER]);
        penSizeEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(markerButton);

    typeDialog = new QWidget();
    typeDialog->setWindowTitle(_("Pen Style"));

    typeButton = create_button(":images/spline.svg", [=](){
        floatingSettings->setPage(0);
        floatingWidget->setFloatingOffset(4);
    });
    typeButton->setStyleSheet(QString("background-color: none;"));
    floatingSettings->addPage(typeDialog);


    QGridLayout *gridLayout = new QGridLayout(typeDialog);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setSpacing(padding);


    lineButton = create_button(":images/line.svg", [=](){
        if(window->penStyle == LINE){
            floatingSettings->hide();
            return;
        }
        if(window->penType == ERASER){
            window->penType = PEN;
        }
        window->penStyle = LINE;
        penStyleEvent();
        set_icon(":images/line.svg", typeButton);
    });
    gridLayout->addWidget(lineButton, 0, 0);

    circleButton = create_button(":images/circle.svg", [=](){
        if(window->penStyle  == CIRCLE){
            floatingSettings->hide();
            return;
        }
        if(window->penType == ERASER){
            window->penType = PEN;
        }
        window->penStyle = CIRCLE;
        penStyleEvent();
        set_icon(":images/circle.svg", typeButton);
    });
    gridLayout->addWidget(circleButton, 0, 1);

    splineButton = create_button(":images/spline.svg", [=](){
        if(window->penStyle == SPLINE){
            floatingSettings->hide();
            return;
        }
        if(window->penType == ERASER){
            window->penType = PEN;
        }
        window->penStyle = SPLINE;
        penStyleEvent();
        set_icon(":images/spline.svg", typeButton);
    });
    gridLayout->addWidget(splineButton, 0, 2);

    typeDialog->setFixedSize(
        (butsize+padding)*3 + padding,
        (butsize+padding)*1 + padding
    );

    eraserButton = create_button(":images/eraser.svg", [=](){
        if(window->penType == ERASER){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        window->penType = ERASER;
        penStyleEvent();
        thicknessSlider->setRange(31,310);
        thicknessSlider->setValue(window->penSize[ERASER]);
        penSizeEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(eraserButton);
    floatingWidget->setWidget(typeButton);
    penStyleEvent();
}

#define addToBackgroundWidget(A) \
    if(A->size().height() > h){ \
        h = A->size().height() + padding; \
    } \
    w += A->size().width(); \
    backgroundLayout->addWidget(A);


static void setupBackground(){
    int w = padding*2;
    int h = padding*2;
    backgroundButton = create_button("",  [=](){
        floatingSettings->setPage(2);
        floatingWidget->setFloatingOffset(5);
    });

    QLabel *pageLabel = new QLabel();
    pageLabel->setText(QString::number(0));
    pageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);


    // main widget
    QWidget *backgroundWidget = new QWidget();
    QVBoxLayout *backgroundMainLayout = new QVBoxLayout(backgroundWidget);
    backgroundMainLayout->setContentsMargins(padding, padding, padding, padding);
    backgroundMainLayout->setSpacing(0);

    backgroundWidget->setStyleSheet(QString("background-color: none;"));
    backgroundButton->setStyleSheet(QString("background-color: none;"));


    // background dialog
    QWidget *backgroundDialog = new QWidget();
    QHBoxLayout *backgroundLayout = new QHBoxLayout(backgroundDialog);
    backgroundLayout->setSpacing(padding);
    backgroundLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    backgroundLayout->setContentsMargins(0, 0, 0, 0);

    
    // background buttons
    transparentButton = create_button(":images/paper-transparent.svg", [=](){
        board->setType(TRANSPARENT);
        backgroundStyleEvent();
    });
    blackButton = create_button(":images/paper-black.svg", [=](){
        board->setType(BLACK);
        backgroundStyleEvent();
    });
    whiteButton = create_button(":images/paper-white.svg", [=](){
        board->setType(WHITE);
        backgroundStyleEvent();
    });

    addToBackgroundWidget(transparentButton);
    addToBackgroundWidget(blackButton);
    addToBackgroundWidget(whiteButton);

    // page dialog
    QWidget *pageDialog = new QWidget();
    QHBoxLayout *pageLayout = new QHBoxLayout(pageDialog);
    pageLayout->setContentsMargins(0, 0, 0, 0);


    pageLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pageLayout->setSpacing(padding);

    // page buttons
    previousPage = create_button(":images/go-page-previous.svg", [=](){
        if(window->getPageNum() == 0){
            return;
        }
        window->goPreviousPage();
        pageLabel->setText(QString::number(window->getPageNum()));
        backgroundStyleEvent();
        updateGoBackButtons();
    });
    previousPage->setStyleSheet(QString("background-color: none;"));

    nextPage = create_button(":images/go-page-next.svg", [=](){
        window->goNextPage();
        backgroundStyleEvent();
        pageLabel->setText(QString::number(window->getPageNum()));
        updateGoBackButtons();
    });
    nextPage->setStyleSheet(QString("background-color: none;"));


    pageLayout->addWidget(previousPage);
    pageLayout->addWidget(pageLabel);
    pageLayout->addWidget(nextPage);


    // overlay dialog
    QWidget *overlayDialog = new QWidget();
    QHBoxLayout *overlayLayout = new QHBoxLayout(overlayDialog);
    overlayLayout->setContentsMargins(0, 0, 0, 0);


    overlayLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    overlayLayout->setSpacing(padding);

    // overlay buttons
    overlayNone = create_button(":images/overlay-none.svg", [=](){
        board->setOverlayType(NONE);
        backgroundStyleEvent();
    });
    overlayNone->setStyleSheet(QString("background-color: none;"));

    overlaySquares = create_button(":images/overlay-squares.svg", [=](){
        board->setOverlayType(SQUARES);
        backgroundStyleEvent();
    });
    overlaySquares->setStyleSheet(QString("background-color: none;"));

    overlayLines = create_button(":images/overlay-lines.svg", [=](){
        board->setOverlayType(LINES);
        backgroundStyleEvent();
    });
    overlayLines->setStyleSheet(QString("background-color: none;"));

    overlayLayout->addWidget(overlayNone);
    overlayLayout->addWidget(overlaySquares);
    overlayLayout->addWidget(overlayLines);

    // set sizes
    pageLabel->setFixedSize(
        blackButton->size().width(),
        blackButton->size().height()
    );


    backgroundDialog->setFixedSize(w,h);
    pageDialog->setFixedSize(w,h);
    overlayDialog->setFixedSize(w,h);
    
    backgroundWidget->setFixedSize(
        w + padding*2,
        h
        + pageDialog->size().height()
        + overlayDialog->size().height()
        + padding*3
    );


    backgroundMainLayout->addWidget(pageDialog);
    backgroundMainLayout->addWidget(backgroundDialog);
    backgroundMainLayout->addWidget(overlayDialog);

    floatingSettings->addPage(backgroundWidget);
    floatingWidget->setWidget(backgroundButton);
    
    backgroundStyleEvent();
}


static void setupGoBackNext(){
    backButton = create_button(":images/go-back.svg", [=](){
        window->goPrevious();
        updateGoBackButtons();
    });
    backButton->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(backButton);
    nextButton = create_button(":images/go-next.svg", [=](){
        window->goNext();
        updateGoBackButtons();
    });
    nextButton->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(nextButton);
    updateGoBackButtons();
}



static void setupMinify(){
    QPushButton *minify = create_button(":images/screen.svg", [=](){
        mainWindow->showMinimized();
    });
    minify->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(minify);

}
static void setupScreenShot(){
    #ifdef screenshot
    QPushButton *ssButton = create_button(":images/screenshot.svg", [=](){
        floatingSettings->hide();
        floatingWidget->hide();
        takeScreenshot();
        floatingWidget->show();
    });
    floatingWidget->setWidget(ssButton);
    ssButton->setStyleSheet(QString("background-color: none;"));
    #endif
}


static void setupClear(){
    QWidget *clearDialog = new QWidget();
    QVBoxLayout *clearLayout = new QVBoxLayout(clearDialog);
    QWidget *clearButtonDialog = new QWidget();
    QHBoxLayout *clearButtonLayout = new QHBoxLayout(clearButtonDialog);

    QLabel *clearLabel = new QLabel();
    char* clearText = _("Are you want to clear screen?");
    clearLabel->setText(clearText);
    clearLabel->setAlignment(Qt::AlignHCenter);
    clearDialog->setStyleSheet(QString("background-color: none;"));

    clearLayout->addWidget(clearLabel);
    clearLayout->addWidget(clearButtonDialog);

    QPushButton * noButton = create_button_text(_("No"), [=](){
        floatingSettings->hide();
    });
    QPushButton * yesButton = create_button_text(_("Yes"), [=](){
        window->clear();
        floatingSettings->hide();
    });
    clearButtonLayout->addWidget(noButton);
    clearButtonLayout->addWidget(yesButton);

    QPushButton *clear = create_button(":images/clear.svg", [=](){
        floatingSettings->setPage(3);
        floatingWidget->setFloatingOffset(7);
    });
    clear->setStyleSheet(QString("background-color: none;"));
    clearDialog->setFixedSize(
        screenWidth * strlen(clearText) / 169,
        screenHeight / 12
    );
    floatingSettings->addPage(clearDialog);
    floatingWidget->setWidget(clear);

}
#ifdef LIBARCHIVE
extern "C" {
static QString archive_target;
void *save_all(void* arg) {
    (void)arg;
    window->saveAll(archive_target);
    return NULL;
}
void *load_archive(void* arg) {
    (void)arg;
    window->loadArchive(archive_target);
    return NULL;
}
}

static void setupSave(){

    QPushButton *save = create_button(":images/save.svg", [=](){
        QString file = QFileDialog::getSaveFileName(window, _("Save File"), QDir::homePath(), _("Pen Files (*.pen);;All Files (*.*)"));
        //window->saveAll(file);
        pthread_t ptid;
        // Creating a new thread
        archive_target = file;
        pthread_create(&ptid, NULL, &save_all, NULL);
    });
    save->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(save);

    QPushButton *open = create_button(":images/open.svg", [=](){
        QString filename = QFileDialog::getOpenFileName(window, _("Open File"), QDir::homePath(), _("Pen Files (*.pen);;All Files (*.*)"));
        if(!filename.isEmpty()){
            pthread_t ptid;
            archive_target = filename;
            pthread_create(&ptid, NULL, &load_archive, NULL);
        }
    });
    open->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(open);

}
#endif

static void setupExit(){
    QWidget *exitDialog = new QWidget();
    QVBoxLayout *exitLayout = new QVBoxLayout(exitDialog);
    QWidget *exitButtonDialog = new QWidget();
    QHBoxLayout *exitButtonLayout = new QHBoxLayout(exitButtonDialog);

    QLabel *exitLabel = new QLabel();
    char* exitText = _("Are you want to quit pardus pen?");
    exitLabel->setText(exitText);
    exitLabel->setAlignment(Qt::AlignHCenter);
    exitLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    exitDialog->setStyleSheet(QString("background-color: none;"));

    exitLayout->addWidget(exitLabel);
    exitLayout->addWidget(exitButtonDialog);

    QPushButton * noButton = create_button_text(_("No"), [=](){
        floatingSettings->hide();
    });
    QPushButton * yesButton = create_button_text(_("Yes"), [=](){
        QApplication::quit();
    });
    exitButtonLayout->addWidget(noButton);
    exitButtonLayout->addWidget(yesButton);

    QPushButton *close = create_button(":images/close.svg", [=](){
        floatingSettings->setPage(4);
        floatingWidget->setFloatingOffset(12);
    });
    close->setStyleSheet(QString("background-color: none;"));
    exitDialog->setFixedSize(
        screenWidth * strlen(exitText) / 169,
        screenHeight / 12
    );
    floatingSettings->addPage(exitDialog);
    floatingWidget->setWidget(close);

}

void setupWidgets(){
    window->floatingSettings = floatingSettings;
    setupMove();
    setupPenType();
    setupPenSize();
    setupBackground();
    setupClear();
    setupMinify();
    setupScreenShot();
    setupGoBackNext();
#ifdef LIBARCHIVE
    setupSave();
#endif
    setupExit();
}
