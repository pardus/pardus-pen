#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>
#include <QMessageBox>
#include <QSlider>
#include <QProcess>


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


extern DrawingWidget *drawing;
extern FloatingWidget *floatingWidget;
extern FloatingSettings *floatingSettings;
extern WhiteBoard *board;
extern QMainWindow* mainWindow;

extern bool fuarMode;

QPushButton *selectButton;

QPushButton *penButton;
QPushButton *typeButton;
QPushButton *markerButton;
QPushButton *eraserButton;
QPushButton *splineButton;
QPushButton *lineButton;
QPushButton *circleButton;
QPushButton *triangleButton;
QPushButton *rectButton;

QPushButton *lineNormalButton;
QPushButton *lineDotLineButton;
QPushButton *lineLineLineButton;

QPushButton *backgroundButton;

QPushButton* fullscreen;

QPushButton* transparentButton;
QPushButton* blackButton;
QPushButton* whiteButton;
QPushButton* colorpicker;

QPushButton* overlayIsometric;
QPushButton* overlayMusic;
QPushButton* overlayCustom;
QPushButton* overlayTurkiye;
QPushButton* overlayLines;
QPushButton* overlaySquares;
QPushButton* overlayNone;


QPushButton *backButton;
QPushButton *nextButton;

QPushButton *previousPage;
QPushButton *nextPage;

OverView *ov;


bool sliderLock = false;

QWidget *penSettings;
QWidget *colorDialog;
QWidget *typeDialog;
QSlider *thicknessSlider;
QLabel *thicknessLabel;
QLabel *colorLabel;

QString penText = "";

extern float scale;

#define butsize 48*scale
#define padding 8*scale

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

static void penStyleEvent(){
    penButton->setStyleSheet(QString("background-color: none;"));
    selectButton->setStyleSheet(QString("background-color: none;"));
    markerButton->setStyleSheet(QString("background-color: none;"));
    eraserButton->setStyleSheet(QString("background-color: none;"));
    lineButton->setStyleSheet(QString("background-color: none;"));
    splineButton->setStyleSheet(QString("background-color: none;"));
    circleButton->setStyleSheet(QString("background-color: none;"));
    rectButton->setStyleSheet(QString("background-color: none;"));
    triangleButton->setStyleSheet(QString("background-color: none;"));
    
    lineDotLineButton->setStyleSheet(QString("background-color: none;"));
    lineNormalButton->setStyleSheet(QString("background-color: none;"));
    lineLineLineButton->setStyleSheet(QString("background-color: none;"));
    switch(drawing->lineStyle){
        case NORMAL:
            lineNormalButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case LINELINE:
            lineLineLineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case DOTLINE:
            lineDotLineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
    }
    switch(drawing->penStyle){
        case LINE:
            lineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/line.svg", typeButton);
            break;
        case CIRCLE:
            circleButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/circle.svg", typeButton);
            break;
        case RECTANGLE:
            rectButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/rectangle.svg", typeButton);
            break;
        case TRIANGLE:
            triangleButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/triangle.svg", typeButton);
            break;
        default:
            splineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/spline.svg", typeButton);
            break;
    }
    if(drawing->penMode == SELECTION) {
        selectButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    } else if(drawing->penMode == DRAW) {
        switch(drawing->penType){
            case PEN:
                penButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
                break;
            case MARKER:
                markerButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
                break;
            default:
                eraserButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
                break;
        }
    }
    ov->penSize = drawing->penSize[drawing->penType];
    ov->penType = drawing->penType;
    ov->color = drawing->penColor;
    ov->updateImage();
}


static int last_pen_type = 0;
static void penSizeEvent(){
    int value = drawing->penSize[drawing->penType];
    if(drawing->penType != last_pen_type){
        last_pen_type = drawing->penType;

        if(drawing->penType == ERASER) {
            ov->setFixedSize(
                colorDialog->size().width(),
                (colorDialog->size().width()*3)/4
            );

            penSettings->setFixedSize(
                colorDialog->size().width() + padding*2,
                padding*2
                 + ov->size().height()
                 + thicknessLabel->size().height()
                 + thicknessSlider->size().height()
            );
            colorDialog->hide();
            //ov->hide();
            colorLabel->hide();
        } else {
            ov->setFixedSize(
                colorDialog->size().width(),
                butsize*3
            );
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
            //ov->show();
        }
    }
    switch(drawing->penType){
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

    thicknessLabel->setText(QString(penText)+QString(" ")+QString(_("Size:"))+QString(" ")+QString::number(value));
    colorLabel->setText(QString(penText)+QString(" ")+QString(_("Color:")));
    ov->penSize = value;
    ov->color = drawing->penColor;
    ov->updateImage();
    floatingSettings->reload();
}

void updateGoBackButtons(){
    if(drawing->isBackAvailable()){
        set_icon(":images/go-back.svg", backButton);
    } else{
        set_icon(":images/go-back-disabled.svg", backButton);
    }
    if(drawing->isNextAvailable()){
        set_icon(":images/go-next.svg", nextButton);
    } else{
        set_icon(":images/go-next-disabled.svg", nextButton);
    }
    if(drawing->getPageNum() == 0){
        set_icon(":images/go-page-previous-disabled.svg", previousPage);
    } else {
        set_icon(":images/go-page-previous.svg", previousPage);
    }
}


static void backgroundStyleEvent(){
    transparentButton->setStyleSheet(QString("background-color: none;"));
    blackButton->setStyleSheet(QString("background-color: none;"));
    whiteButton->setStyleSheet(QString("background-color: none;"));
    
    overlayIsometric->setStyleSheet(QString("background-color: none;"));
    overlayMusic->setStyleSheet(QString("background-color: none;"));
    overlayLines->setStyleSheet(QString("background-color: none;"));
    overlaySquares->setStyleSheet(QString("background-color: none;"));
    overlayNone->setStyleSheet(QString("background-color: none;"));
    overlayCustom->setStyleSheet(QString("background-color: none;"));
    overlayTurkiye->setStyleSheet(QString("background-color: none;"));
    drawing->cropWidget->setStyleSheet("border: 2px solid "+drawing->penColor.name()+";");
    switch(board->getType()){
        case BLACK:
            set_icon(":images/paper-black.svg",backgroundButton);
            blackButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            ov->background = Qt::black;
            break;
        case WHITE:
            whiteButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/paper-white.svg",backgroundButton);
            ov->background = Qt::white;
            break;
        default:
            transparentButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            set_icon(":images/paper-transparent.svg",backgroundButton);
            ov->background = Qt::transparent;
            break;
    }
    switch(board->getOverlayType()){
        
        case LINES:
            overlayLines->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case ISOMETRIC:
            overlayIsometric->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case MUSIC:
            overlayMusic->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case SQUARES:
            overlaySquares->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
       case CUSTOM:
            overlayCustom->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case TURKIYE:
            overlayTurkiye->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        default:
            overlayNone->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    }
    ov->updateImage();
}


static void setupMove(){
    QLabel *move = new QLabel("");
    QIcon icon = QIcon(":images/move-icon.svg");
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(butsize, butsize)));
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
            "height: "+QString::number(22*scale)+"px;"
            "border-radius: "+QString::number(11*scale)+"px;"
        "}"
        "QSlider::handle:horizontal {"
            "background: #fff;"
            "border: 1px solid #777;"
            "width: "+QString::number(44*scale)+"px;"
            "margin: -4px 0;"
            "border-radius: "+QString::number(18*scale)+"px;"
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
            "border-radius:5px;"
        "}"
    );

    ov->updateImage();
    penSettingsLayout->addWidget(ov);

    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setRange(1,50*scale);
    thicknessSlider->setSingleStep(1);
    thicknessSlider->setValue(drawing->penSize[PEN]);


    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penSettingsLayout->setSpacing(0);


    thicknessLabel = new QLabel();
    thicknessLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); 
 
    penSettingsLayout->addWidget(thicknessLabel);
    penSettingsLayout->addWidget(thicknessSlider);

    penSettings->show();

    QObject::connect(thicknessSlider, &QSlider::valueChanged, [=](int value) {
        if(!sliderLock){
            drawing->penSize[drawing->penType] = value;
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
        QColor newCol = QColorDialog::getColor(drawing->penColor, mainWindow, _("Select Color"));
        if(! newCol.isValid()){
            return;
        }
        drawing->penColor = newCol;
        set_string((char*)"color", (char*)drawing->penColor.name().toStdString().c_str());
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
            drawing->penColor = colors[i];
            set_string((char*)"color", (char*)drawing->penColor.name().toStdString().c_str());
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
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        if(floatingSettings->isVisible() && drawing->penType == PEN){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        drawing->penType = PEN;
        drawing->penStyle = SPLINE;
        thicknessSlider->setRange(1,50*scale);
        thicknessSlider->setValue(drawing->penSize[PEN]);
        penSizeEvent();
        penStyleEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(penButton);
    
    selectButton = create_button(":images/crop.svg", [=](){
        drawing->penMode = SELECTION;
        penStyleEvent();
    });
    floatingWidget->setWidget(selectButton);

    markerButton = create_button(":images/marker.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        if(floatingSettings->isVisible() && drawing->penType == MARKER){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        drawing->penType = MARKER;
        drawing->penStyle = SPLINE;
        thicknessSlider->setRange(1,50*scale);
        thicknessSlider->setValue(drawing->penSize[MARKER]);
        penSizeEvent();
        penStyleEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(markerButton);

    eraserButton = create_button(":images/eraser.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        if(floatingSettings->isVisible() && drawing->penType == ERASER){
            floatingSettings->hide();
            return;
        }
        sliderLock = true;
        drawing->penType = ERASER;
        penStyleEvent();
        thicknessSlider->setRange(10*scale,200*scale);
        thicknessSlider->setValue(drawing->penSize[ERASER]);
        penSizeEvent();
        sliderLock = false;
    });
    floatingWidget->setWidget(eraserButton);

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
        if(drawing->penStyle == LINE){
            floatingSettings->hide();
            return;
        }
        if(drawing->penType == ERASER){
            drawing->penType = PEN;
        }
        drawing->penStyle = LINE;
        penStyleEvent();
    });
    gridLayout->addWidget(lineButton, 0, 0);

    circleButton = create_button(":images/circle.svg", [=](){
        if(drawing->penStyle  == CIRCLE){
            floatingSettings->hide();
            return;
        }
        if(drawing->penType == ERASER){
            drawing->penType = PEN;
        }
        drawing->penStyle = CIRCLE;
        penStyleEvent();
    });
    gridLayout->addWidget(circleButton, 0, 1);

    triangleButton = create_button(":images/triangle.svg", [=](){
        if(drawing->penStyle  == TRIANGLE){
            floatingSettings->hide();
            return;
        }
        if(drawing->penType == ERASER){
            drawing->penType = PEN;
        }
        drawing->penStyle = TRIANGLE;
        penStyleEvent();
    });
    gridLayout->addWidget(triangleButton, 1, 1);

    rectButton = create_button(":images/rectangle.svg", [=](){
        if(drawing->penStyle  == RECTANGLE){
            floatingSettings->hide();
            return;
        }
        if(drawing->penType == ERASER){
            drawing->penType = PEN;
        }
        drawing->penStyle = RECTANGLE;
        penStyleEvent();
    });
    gridLayout->addWidget(rectButton, 1, 0);

    splineButton = create_button(":images/spline.svg", [=](){
        if(drawing->penStyle == SPLINE){
            floatingSettings->hide();
            return;
        }
        if(drawing->penType == ERASER){
            drawing->penType = PEN;
        }
        drawing->penStyle = SPLINE;
        penStyleEvent();
    });
    gridLayout->addWidget(splineButton, 0, 2);

    lineNormalButton = create_button(":images/line-normal.svg", [=](){
        drawing->lineStyle = NORMAL;
        penStyleEvent();
    });
    gridLayout->addWidget(lineNormalButton, 2, 0);

    lineDotLineButton = create_button(":images/line-dotline.svg", [=](){
        drawing->lineStyle = DOTLINE;
        penStyleEvent();
    });
    gridLayout->addWidget(lineDotLineButton, 2, 1);
    
    lineLineLineButton = create_button(":images/line-lineline.svg", [=](){
        drawing->lineStyle = LINELINE;
        penStyleEvent();
    });
    gridLayout->addWidget(lineLineLineButton, 2, 2);

    typeDialog->setFixedSize(
        (butsize+padding)*3 + padding,
        (butsize+padding)*3 + padding
    );

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
        if(drawing->getPageNum() == 0){
            return;
        }
        drawing->goPreviousPage();
        pageLabel->setText(QString::number(drawing->getPageNum()));
        backgroundStyleEvent();
        updateGoBackButtons();
    });
    previousPage->setStyleSheet(QString("background-color: none;"));

    nextPage = create_button(":images/go-page-next.svg", [=](){
        drawing->goNextPage();
        backgroundStyleEvent();
        pageLabel->setText(QString::number(drawing->getPageNum()));
        updateGoBackButtons();
    });
    nextPage->setStyleSheet(QString("background-color: none;"));


    pageLayout->addWidget(previousPage);
    pageLayout->addWidget(pageLabel);
    pageLayout->addWidget(nextPage);


    // overlay dialog
    QWidget *overlayDialog = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(overlayDialog);
    gridLayout->setContentsMargins(0, 0, 0, 0);


    gridLayout->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->setSpacing(padding);

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

    overlayIsometric = create_button(":images/overlay-isometric.svg", [=](){
        board->setOverlayType(ISOMETRIC);
        backgroundStyleEvent();
    });
    overlayIsometric->setStyleSheet(QString("background-color: none;"));

    overlayMusic = create_button(":images/overlay-music.svg", [=](){
        board->setOverlayType(MUSIC);
        backgroundStyleEvent();
    });
    overlayMusic->setStyleSheet(QString("background-color: none;"));
    
    overlayCustom = create_button(":images/overlay-custom.svg", [=](){
        board->setOverlayType(CUSTOM);
        backgroundStyleEvent();
    });
    overlayCustom->setStyleSheet(QString("background-color: none;"));

    overlayTurkiye = create_button(":images/turkiye-map.svg", [=](){
        board->setOverlayType(TURKIYE);
        backgroundStyleEvent();
    });
    overlayTurkiye->setStyleSheet(QString("background-color: none;"));
    
    fullscreen = create_button(":images/fullscreen-exit.svg", [=](){
        if(mainWindow->isFullScreen()){
            set_icon(":images/fullscreen.svg", fullscreen);
            mainWindow->showNormal();
        } else {
            set_icon(":images/fullscreen-exit.svg", fullscreen);
            mainWindow->showFullScreen();
        }
    });
    fullscreen->setStyleSheet(QString("background-color: none;"));


    gridLayout->addWidget(overlayNone, 0, 0, Qt::AlignCenter);
    gridLayout->addWidget(overlaySquares, 0, 1, Qt::AlignCenter);
    gridLayout->addWidget(overlayLines, 1, 0, Qt::AlignCenter);
    gridLayout->addWidget(overlayIsometric, 1, 1, Qt::AlignCenter);
    gridLayout->addWidget(overlayMusic, 0, 2, Qt::AlignCenter);
    gridLayout->addWidget(overlayCustom, 1, 2, Qt::AlignCenter);
    gridLayout->addWidget(overlayTurkiye, 2, 0, Qt::AlignCenter);
    
    
    gridLayout->addWidget(fullscreen, 2, 1, Qt::AlignCenter);

    // set sizes
    pageLabel->setFixedSize(
        blackButton->size().width(),
        blackButton->size().height()
    );


    backgroundDialog->setFixedSize(w,h);
    pageDialog->setFixedSize(w,h);
    overlayDialog->setFixedSize(w,h*3);
    
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
        drawing->goPrevious();
        updateGoBackButtons();
    });
    backButton->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(backButton);
    nextButton = create_button(":images/go-next.svg", [=](){
        drawing->goNext();
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
    char* clearText = _("Do you want to clear screen?");
    clearLabel->setText(clearText);
    clearLabel->setAlignment(Qt::AlignHCenter);
    clearDialog->setStyleSheet(QString("background-color: none;"));

    clearLayout->addWidget(clearLabel);
    clearLayout->addWidget(clearButtonDialog);

    QPushButton * noButton = create_button_text(_("No"), [=](){
        floatingSettings->hide();
    });
    QPushButton * yesButton = create_button_text(_("Yes"), [=](){
        drawing->clear();
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
        butsize * 10,
        butsize * 2
    );
    floatingSettings->addPage(clearDialog);
    floatingWidget->setWidget(clear);

}
#ifdef LIBARCHIVE
extern "C" {
QString archive_target;
void *save_all(void* arg) {
    (void)arg;
    drawing->saveAll(archive_target);
    return NULL;
}
void *load_archive(void* arg) {
    (void)arg;
    drawing->loadArchive(archive_target);
    return NULL;
}
}

static void setupSave(){

    QPushButton *save = create_button(":images/save.svg", [=](){
        QString filter = _("Pen Files (*.pen);;");
        #ifdef QPRINTER
        filter += "PDF Files (*.pdf);;";
        #endif
        filter += _("All Files (*.*)");
        QString file = QFileDialog::getSaveFileName(drawing, _("Save File"), QDir::homePath(), filter);
        //drawing->saveAll(file);
        pthread_t ptid;
        // Creating a new thread
        archive_target = file;
        pthread_create(&ptid, NULL, &save_all, NULL);
    });
    save->setStyleSheet(QString("background-color: none;"));
    floatingWidget->setWidget(save);

    QPushButton *open = create_button(":images/open.svg", [=](){
        QString filename = QFileDialog::getOpenFileName(drawing, _("Open File"), QDir::homePath(), _("Pen Files (*.pen);;All Files (*.*)"));
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
    char* exitText = _("Do you want to quit Pardus pen?");
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
#ifdef ETAP19
    QStringList args3;
    QProcess p3;
    args3 << "-d" << "eta-disable-gestures@pardus.org.tr";
    p3.execute("gnome-shell-extension-tool", args3);

    QStringList args4;
    QProcess p4;
    args4 << "set" << "org.gnome.mutter" << "overlay-key" << "'SUPER_L'";
    p4.execute("gsettings", args4);
#endif
        QApplication::quit();
        exit(0);
    });
    exitButtonLayout->addWidget(noButton);
    exitButtonLayout->addWidget(yesButton);

    QPushButton *close = create_button(":images/close.svg", [=](){
        floatingSettings->setPage(4);
        floatingWidget->setFloatingOffset(12);
    });
    close->setStyleSheet(QString("background-color: none;"));
    exitDialog->setFixedSize(
        butsize * 10,
        butsize * 2
    );
    floatingSettings->addPage(exitDialog);
    floatingWidget->setWidget(close);

}

void setupWidgets(){
    drawing->floatingSettings = floatingSettings;
    setupMove();
    setupPenType();
    setupPenSize();
    setupBackground();
    setupClear();
    if(!fuarMode){
        setupMinify();
    }
    setupScreenShot();
    setupGoBackNext();
#ifdef LIBARCHIVE
    setupSave();
#endif
    if(!fuarMode){
        setupExit();
    }
}
