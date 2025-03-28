#include "tools.h"
#include "tools/color.h"

#include <dirent.h>

QWidget *colorDialog;

QLabel *thicknessLabel;

QWidget *penTypeDialog;
QWidget *modeDialog;

static QVBoxLayout *penSettingsLayout;
static QVBoxLayout *pageSettingsLayout;
static QVBoxLayout *shapeSettingsLayout;

void setupWidgets(){
    // Pen Settings Menu
    QWidget *penSettings = new QWidget();
    penSettingsLayout = new QVBoxLayout(penSettings);
    penSettingsLayout->setSpacing(padding);
    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penSettings->setStyleSheet(QString("background-color: none;"));
    floatingSettings->addPage(penSettings);

    // Pen button with menu
    toolButtons[PENMENU] = create_button(":images/pen.svg", [=](){
        if(drawing->getPen() != PEN){
            setPen(PEN);
            return;
        }
        floatingSettings->setPage(0);
        floatingWidget->moveAction();
    });

    // Eraser button with menu
    toolButtons[ERASERMENU] = create_button(":images/eraser.svg", [=](){
        if(drawing->getPen() != ERASER){
            setPen(ERASER);
            return;
        }
        floatingSettings->setPage(0);
        floatingWidget->moveAction();
    });

    // Page settings menu
    QWidget *pageSettings = new QWidget();
    pageSettings->setStyleSheet(QString("background-color: none;"));
    pageSettingsLayout = new QVBoxLayout(pageSettings);
    pageSettingsLayout->setSpacing(0);
    pageSettingsLayout->setContentsMargins(0, 0, 0, 0);
    floatingSettings->addPage(pageSettings);

    // Page menu button
    toolButtons[PAGEMENU] = create_button(":images/page-settings.svg", [=](){
           floatingSettings->setPage(1);
           floatingWidget->moveAction();
    });

    // Shape settings menu
    QWidget *shapeSettings = new QWidget();
    shapeSettings->setStyleSheet(QString("background-color: none;"));
    shapeSettingsLayout = new QVBoxLayout(shapeSettings);
    shapeSettingsLayout->setSpacing(0);
    shapeSettingsLayout->setContentsMargins(0, 0, 0, 0);
    floatingSettings->addPage(shapeSettings);

    // Page menu button
    toolButtons[SHAPEMENU] = create_button(":images/spline.svg", [=](){
           floatingSettings->setPage(2);
           floatingWidget->moveAction();
    });

    floatingSettings->setHide();


/********** Main toolbar **********/

    floatingWidget->addWidget(toolButtons[PENMENU]);
    floatingWidget->addWidget(toolButtons[ERASERMENU]);
    floatingWidget->addWidget(toolButtons[SHAPEMENU]);
    floatingWidget->addWidget(toolButtons[PAGEMENU]);
    floatingWidget->addWidget(toolButtons[BACK]);
    floatingWidget->addWidget(toolButtons[NEXT]);
    floatingWidget->addWidget(toolButtons[CLEAR]);
    floatingWidget->addWidget(toolButtons[MINIFY]);
    floatingWidget->addWidget(create_color_button(QColor("#0078d7")));
    floatingWidget->addWidget(create_color_button(QColor("#00ae4d")));
    floatingWidget->addWidget(create_color_button(QColor("#ffc000")));
    floatingWidget->addWidget(create_color_button(QColor("#ff0000")));
    // disable minify if fual enabled
    toolButtons[MINIFY]->setEnabled(!get_bool("fuar"));

/*********** main menu done *********/


/************************************/
/************ Pen Menu **************/

/********** Pen Settings **********/
    // color dialog
    int num_of_color = sizeof(colors) / sizeof(QColor);
    int rowsize = 7;
    colorDialog = new QWidget();
    colorDialog->setFixedSize( // set height later
        butsize*rowsize + padding*(rowsize+1),
        0
    );


/********** Overview **********/

    penSettingsLayout->addWidget(ov);


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


/********** penTypes **********/
    QWidget *stylDialog = new QWidget();
    QGridLayout *styleLayout = new QGridLayout(stylDialog);
    // spline
    styleLayout->addWidget(penButtons[PEN],          0, 0, Qt::AlignCenter);
    styleLayout->addWidget(penButtons[ERASER],       0, 1, Qt::AlignCenter);
    styleLayout->addWidget(penButtons[MARKER],       0, 2, Qt::AlignCenter);
    styleLayout->addWidget(penButtons[SELECTION],    0, 3, Qt::AlignCenter);
    styleLayout->addWidget(toolButtons[COLORPICKER], 0, 4, Qt::AlignCenter);

    penSettingsLayout->addWidget(stylDialog);


/********** Color selection options **********/
    // color selection

    QGridLayout *gridLayout = new QGridLayout(colorDialog);

    // Create buttons for each color

    // Color button offset is 100
    for (int i = 0; i < num_of_color; i++) {
        toolButtons[i+100] = create_color_button(colors[i]);
        gridLayout->addWidget(toolButtons[i+100], i / rowsize, i % rowsize, Qt::AlignCenter);
    }
    colorDialog->setLayout(gridLayout);
    penSettingsLayout->addWidget(colorDialog);

    // sync overview size with colordialog
    ov->setFixedSize(
        colorDialog->size().width(),
        colorDialog->size().width()/2
    );

/*********** Pen menu done *********/
/***********************************/
/*********** Shape menu  ***********/

/********** penModes **********/
    modeDialog = new QWidget();
    QGridLayout *modeLayout = new QGridLayout(modeDialog);
    // spline
    modeLayout->addWidget(penButtons[SPLINE],     0, 0, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[LINE],       0, 1, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[CIRCLE],     0, 2, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[TRIANGLE],   0, 3, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[RECTANGLE],  0, 4, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[VECTOR],     1, 0, Qt::AlignCenter);
    modeLayout->addWidget(penButtons[VECTOR2],    1, 1);

    shapeSettingsLayout->addWidget(modeDialog);


/********** pen type **********/

    penTypeDialog = new QWidget();
    QGridLayout *penTypeLayout = new QGridLayout(penTypeDialog);
    penTypeLayout->addWidget(penButtons[NORMAL],   0, 0, Qt::AlignCenter);
    penTypeLayout->addWidget(penButtons[DOTLINE],  0, 1, Qt::AlignCenter);
    penTypeLayout->addWidget(penButtons[LINELINE], 0, 2, Qt::AlignCenter);

    shapeSettingsLayout->addWidget(penTypeDialog);


    // resize color dialog
    colorDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*4+ padding*5
    );

/*********** Shape menu done *********/
/***********************************/
/************ Page Menu ************/

 /********** page number **********/

    QWidget *pageNumWidget = new QWidget();
    QHBoxLayout *pageNumLayout = new QHBoxLayout(pageNumWidget);

    pageNumLayout->addWidget(new QLabel(_("Page:")));
    pageNumLayout->addWidget(pageLabel);
    pageNumLayout->addWidget(toolButtons[PREVPAGE]);
    pageNumLayout->addWidget(toolButtons[NEXTPAGE]);

    QLabel *vsep1 = new QLabel();
    vsep1->setStyleSheet("background: black;");
    vsep1->setFixedSize(
        1,
        butsize
    );
    pageNumLayout->addWidget(vsep1);

    pageNumLayout->addWidget(penButtons[TRANSPARENT]);
    pageNumLayout->addWidget(penButtons[BLACK]);
    pageNumLayout->addWidget(penButtons[WHITE]);

    pageSettingsLayout->addWidget(pageNumWidget);

/********** page type **********/

    QLabel *bgLabel = new QLabel(_("Background:"));
    bgLabel->setStyleSheet("background: none;");
    bgLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pageSettingsLayout->addWidget(bgLabel);

    QWidget *pageDialog = new QWidget();
    QGridLayout *pageLayout = new QGridLayout(pageDialog);
    // spline
    pageLayout->addWidget(penButtons[BLANK],     0, 0, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[SQUARES],   0, 1, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[LINES],     0, 2, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[MUSIC],     0, 3, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[CUSTOM],    0, 4, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[ISOMETRIC], 0, 5, Qt::AlignCenter);

    struct dirent *ep;
    DIR *dp = opendir (BGDIR);
    int i = 6;
    // custom overlay button offset is 200
    if (dp != NULL) {
        while ((ep = readdir (dp)) != NULL) {
            if ((ep->d_name)[0] == '.') {
                continue;
            }
            QString path = QString(BGDIR) + QString("/") + QString(ep->d_name);
            toolButtons[i+200] = create_button(path.toStdString().c_str(), [=](){
                board->overlays[drawing->getPageNum()] = QImage(path);
                board->setOverlayType(CUSTOM);
                board->rotates[drawing->getPageNum()] = 0;
                board->ratios[drawing->getPageNum()] = 100;
                board->updateTransform();
            });
            pageLayout->addWidget(toolButtons[i+200], i / 6, i % 6, Qt::AlignCenter);
            i++;
            printf ("%s\n", ep->d_name);
        }
        closedir(dp);
    }
    pageDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*2+ padding*3
    );
    pageSettingsLayout->addWidget(pageDialog);


/********** clear & screenshot **********/

    QWidget *miscDialog = new QWidget();
    QGridLayout *miscLayout = new QGridLayout(miscDialog);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEUP],    0, 0, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYSCALEUP],     0, 1, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYSCALEDOWN],   0, 2, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEDOWN],  0, 3, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[SAVE],               0, 4, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OPEN],               0, 5, Qt::AlignCenter);

    miscLayout->addWidget(toolButtons[CLOSE],        1, 0, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[SCREENSHOT],   1, 1, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[FULLSCREEN],   1, 2, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[ROTATE],       1, 3, Qt::AlignCenter);
    if(get_bool("fuar")){
        toolButtons[SAVE]->setEnabled(false);
        toolButtons[OPEN]->setEnabled(false);
        toolButtons[CLOSE]->setEnabled(false);
        toolButtons[SAVE]->setEnabled(false);
    }

    pageSettingsLayout->addWidget(miscDialog);


/********** Finish him **********/


    penStyleEvent();
    penSizeEvent();
    backgroundStyleEvent();
    updateGoBackButtons();
    ov->updateImage();


}
