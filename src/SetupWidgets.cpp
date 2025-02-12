#include "tools.h"
#include "tools/color.h"

#include <dirent.h>

QPushButton *penMenu;
QPushButton *toolMenu;
QWidget *colorDialog;

QLabel *thicknessLabel;

QWidget *penTypeDialog;
QWidget *modeDialog;

static QVBoxLayout *penSettingsLayout;
static QVBoxLayout *toolSettingsLayout;

void create_seperator(bool f){
    QLabel *hlina = new QLabel();
    hlina->setStyleSheet("background: black;");
    hlina->setFixedSize(
        colorDialog->size().width(),
        1
    );
    if(f){
        penSettingsLayout->addWidget(hlina);
    } else {
        toolSettingsLayout->addWidget(hlina);
    }
 
}

void setupWidgets(){
    // Pen Menu
    QWidget *penSettings = new QWidget();
    penSettingsLayout = new QVBoxLayout(penSettings);
    penSettingsLayout->setSpacing(padding);
    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penMenu = create_button(":images/pen-menu.svg", [=](){
           floatingSettings->setPage(0);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(penSettings);

    // tool Menu
    QWidget *toolSettings = new QWidget();
    toolSettingsLayout = new QVBoxLayout(toolSettings);
    toolSettingsLayout->setSpacing(0);
    toolSettingsLayout->setContentsMargins(0, 0, 0, 0);
    toolMenu = create_button(":images/pen-settings.svg", [=](){
           floatingSettings->setPage(1);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(toolSettings);

    floatingSettings->setHide();


/********** Main toolbar **********/

    floatingWidget->addWidget("move", move);
    floatingWidget->addWidget("pen-menu", penMenu);
    floatingWidget->addWidget("pen-switch", penSwitch);
    floatingWidget->addWidget("next", nextButton);
    floatingWidget->addWidget("back", backButton);
    floatingWidget->addWidget("tool-menu", toolMenu);
    if(!get_bool((char*)"fuar")){
        floatingWidget->addWidget("minify", minify);
    }

/*********** main menu done *********/
/************************************/
/************ Pen Menu **************/

/********** Pen Settings **********/
    // color dialog
    int num_of_color = sizeof(colors) / sizeof(QColor);
    int rowsize = 7;
    colorDialog = new QWidget();
    colorDialog->setFixedSize( // set height later
        butsize*rowsize + padding*(rowsize),
        0
    );


/********** Overview **********/

    penSettingsLayout->addWidget(ov);
    create_seperator(true);

/********** penTypes **********/
    QWidget *stylDialog = new QWidget();
    QGridLayout *styleLayout = new QGridLayout(stylDialog);
    // spline
    styleLayout->addWidget(penButton,    0, 0);
    styleLayout->addWidget(eraserButton, 0, 1);
    styleLayout->addWidget(markerButton, 0, 2);
    styleLayout->addWidget(selectButton,  0, 3);
    
    penSettingsLayout->addWidget(stylDialog);

    create_seperator(true);

/********** Color selection options **********/
    // color selection
    
    QGridLayout *gridLayout = new QGridLayout(colorDialog);
    
    // Create buttons for each color

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
            penSizeEvent();
            backgroundStyleEvent();
        });
        gridLayout->addWidget(button, (i+1) / rowsize, (i+1) % rowsize, Qt::AlignCenter);
    }
    colorDialog->setLayout(gridLayout);
    penSettingsLayout->addWidget(colorDialog);
    
    // sync overview size with colordialog
    ov->setFixedSize(
        colorDialog->size().width(),
        colorDialog->size().width()/2
    );

    create_seperator(true);


/********** Thickness slider **********/
    
    thicknessLabel = new QLabel(_("Size: 10"));
    thicknessLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    penSettingsLayout->addWidget(thicknessLabel);

    thicknessLabel->setFixedSize(
        colorDialog->size().width(),
        butsize / 2
    );

    penSettingsLayout->addWidget(thicknessSlider);
    thicknessSlider->setFixedSize(
        colorDialog->size().width(),
        butsize
    );

    thicknessSlider->setStyleSheet(
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

    create_seperator(true);

/********** penModes **********/
    modeDialog = new QWidget();
    QGridLayout *modeLayout = new QGridLayout(modeDialog);
    // spline
    modeLayout->addWidget(splineButton,     0, 0);
    modeLayout->addWidget(lineButton,       0, 1);
    modeLayout->addWidget(circleButton,     0, 2);
    modeLayout->addWidget(triangleButton,   0, 3);
    modeLayout->addWidget(rectButton,       0, 4);
    
    penSettingsLayout->addWidget(modeDialog);

    create_seperator(true);

/********** pen type **********/

    penTypeDialog = new QWidget();
    QGridLayout *penTypeLayout = new QGridLayout(penTypeDialog);
    penTypeLayout->addWidget(lineNormalButton,      0, 0);
    penTypeLayout->addWidget(lineDotLineButton,      0, 1);
    penTypeLayout->addWidget(lineLineLineButton,       0, 2);
    
    penSettingsLayout->addWidget(penTypeDialog);

    create_seperator(true);

    // resize color dialog
    colorDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*4+ padding*3
    );

/*********** Pen menu done *********/
/***********************************/
/************ Tool Menu ************/

/********** page type **********/ 

    QLabel *bgLabel = new QLabel(_("Background:"));
    bgLabel->setStyleSheet("background: none;");
    bgLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    toolSettingsLayout->addWidget(bgLabel);

    QWidget *pageDialog = new QWidget();
    QGridLayout *pageLayout = new QGridLayout(pageDialog);
    // spline
    pageLayout->addWidget(overlayNone,      0, 0);
    pageLayout->addWidget(overlaySquares,   0, 1);
    pageLayout->addWidget(overlayLines,     0, 2);
    pageLayout->addWidget(overlayMusic,     0, 3);
    pageLayout->addWidget(overlayCustom,    1, 0);
    pageLayout->addWidget(overlayIsometric, 1, 1);

    struct dirent *ep;
    DIR *dp = opendir (BGDIR);
    int i = 6;
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL) {
            if ((ep->d_name)[0] == '.') {
                continue;
            }
            QString path = QString(BGDIR) + QString("/") + QString(ep->d_name);
            QPushButton* but = create_button(path.toStdString().c_str(), [=](){
                board->overlays[drawing->getPageNum()] = QImage(path);
                board->setOverlayType(CUSTOM);
            });
            pageLayout->addWidget(but, i / 4, i % 4);
            i++;
            printf ("%s\n", ep->d_name);
        }
    }
    closedir(dp);
    pageDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*2+ padding*3
    );
    toolSettingsLayout->addWidget(pageDialog);

    create_seperator(false);


 /********** page number **********/

    QWidget *pageNumWidget = new QWidget();
    QHBoxLayout *pageNumLayout = new QHBoxLayout(pageNumWidget);

    pageNumLayout->addWidget(new QLabel(_("Page:")));
    pageNumLayout->addWidget(pageLabel);
    pageNumLayout->addWidget(previousPage);
    pageNumLayout->addWidget(nextPage);

    QLabel *vsep1 = new QLabel();
    vsep1->setStyleSheet("background: black;");
    vsep1->setFixedSize(
        1,
        butsize
    );
    pageNumLayout->addWidget(vsep1);

    pageNumLayout->addWidget(transparentButton);
    pageNumLayout->addWidget(blackButton);
    pageNumLayout->addWidget(whiteButton);

    toolSettingsLayout->addWidget(pageNumWidget);

    create_seperator(false);

/********** clear & screenshot **********/

    QWidget *miscDialog = new QWidget();
    QGridLayout *miscLayout = new QGridLayout(miscDialog);
    miscLayout->addWidget(clear,      0, 0);
    miscLayout->addWidget(ssButton,   0, 1);
    if(!get_bool((char*)"fuar")){
        miscLayout->addWidget(close,      0, 2);
        miscLayout->addWidget(save,       0, 3);
        miscLayout->addWidget(open,       0, 4);
        miscLayout->addWidget(fullscreen, 0, 5);
        miscLayout->addWidget(rotate,     0, 6);
    }
    miscLayout->addWidget(overlayScaleUp,     1, 0);
    miscLayout->addWidget(overlayScaleDown,   1, 1);
    miscLayout->addWidget(overlayRotateUp,     1, 2);
    miscLayout->addWidget(overlayRotateDown,   1, 3);

    toolSettingsLayout->addWidget(miscDialog);


/********** Finish him **********/


    penStyleEvent();
    penSizeEvent();
    backgroundStyleEvent();
    updateGoBackButtons();
    ov->updateImage();


}
