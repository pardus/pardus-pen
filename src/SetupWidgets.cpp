#include "tools.h"
#include "tools/color.h"

#include <dirent.h>

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
    toolButtons[PENMENU] = create_button(":images/pen-menu.svg", [=](){
           floatingSettings->setPage(0);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(penSettings);

    // tool Menu
    QWidget *toolSettings = new QWidget();
    toolSettingsLayout = new QVBoxLayout(toolSettings);
    toolSettingsLayout->setSpacing(0);
    toolSettingsLayout->setContentsMargins(0, 0, 0, 0);
    toolButtons[TOOLMENU] = create_button(":images/pen-settings.svg", [=](){
           floatingSettings->setPage(1);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(toolSettings);

    floatingSettings->setHide();


/********** Main toolbar **********/

    floatingWidget->addWidget("move", move);
    floatingWidget->addWidget("pen-menu", toolButtons[PENMENU]);
    floatingWidget->addWidget("pen-switch", toolButtons[SWITCH]);
    floatingWidget->addWidget("next", toolButtons[NEXT]);
    floatingWidget->addWidget("back", toolButtons[BACK]);
    floatingWidget->addWidget("tool-menu", toolButtons[TOOLMENU]);
    if(!get_bool("fuar")){
        floatingWidget->addWidget("minify", toolButtons[MINIFY]);
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
    styleLayout->addWidget(penButtons[PEN],    0, 0);
    styleLayout->addWidget(penButtons[ERASER], 0, 1);
    styleLayout->addWidget(penButtons[MARKER], 0, 2);
    styleLayout->addWidget(penButtons[SELECTION],  0, 3);
    
    penSettingsLayout->addWidget(stylDialog);

    create_seperator(true);

/********** Color selection options **********/
    // color selection
    
    QGridLayout *gridLayout = new QGridLayout(colorDialog);
    
    // Create buttons for each color

    gridLayout->addWidget(toolButtons[COLORPICKER], 0, 0, Qt::AlignCenter);
    // Color button offset is 100
    for (int i = 0; i < num_of_color; i++) {
        toolButtons[i+100] = new QPushButton(colorDialog);
        toolButtons[i+100]->setFixedSize(butsize, butsize);
        toolButtons[i+100]->setStyleSheet(QString(
             "background-color: %1;"
             "border-radius: 12px;"
             "border: 1px solid "+convertColor(colors[i]).name()+";"
        ).arg(colors[i].name()));
        QObject::connect(toolButtons[i+100], &QPushButton::clicked, [=]() {
            drawing->penColor = colors[i];
            set_string("color", drawing->penColor.name());
            penStyleEvent();
            penSizeEvent();
            backgroundStyleEvent();
        });
        gridLayout->addWidget(toolButtons[i+100], (i+1) / rowsize, (i+1) % rowsize, Qt::AlignCenter);
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
    modeLayout->addWidget(penButtons[SPLINE],     0, 0);
    modeLayout->addWidget(penButtons[LINE],       0, 1);
    modeLayout->addWidget(penButtons[CIRCLE],     0, 2);
    modeLayout->addWidget(penButtons[TRIANGLE],   0, 3);
    modeLayout->addWidget(penButtons[RECTANGLE],       0, 4);
    modeLayout->addWidget(penButtons[VECTOR],     1, 0);
    modeLayout->addWidget(penButtons[VECTOR2],    1, 1);

    penSettingsLayout->addWidget(modeDialog);

    create_seperator(true);

/********** pen type **********/

    penTypeDialog = new QWidget();
    QGridLayout *penTypeLayout = new QGridLayout(penTypeDialog);
    penTypeLayout->addWidget(penButtons[NORMAL],      0, 0);
    penTypeLayout->addWidget(penButtons[DOTLINE],      0, 1);
    penTypeLayout->addWidget(penButtons[LINELINE],       0, 2);
    
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
    pageLayout->addWidget(penButtons[BLANK],      0, 0);
    pageLayout->addWidget(penButtons[SQUARES],   0, 1);
    pageLayout->addWidget(penButtons[LINES],     0, 2);
    pageLayout->addWidget(penButtons[MUSIC],     0, 3);
    pageLayout->addWidget(penButtons[CUSTOM],    1, 0);
    pageLayout->addWidget(penButtons[ISOMETRIC], 1, 1);

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
            });
            pageLayout->addWidget(toolButtons[i+200], i / 4, i % 4);
            i++;
            printf ("%s\n", ep->d_name);
        }
        closedir(dp);
    }
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

    toolSettingsLayout->addWidget(pageNumWidget);

    create_seperator(false);

/********** clear & screenshot **********/

    QWidget *miscDialog = new QWidget();
    QGridLayout *miscLayout = new QGridLayout(miscDialog);
    miscLayout->addWidget(toolButtons[CLEAR],      0, 0);
    miscLayout->addWidget(toolButtons[SCREENSHOT],   0, 1);
    if(!get_bool("fuar")){
        miscLayout->addWidget(toolButtons[CLOSE],        0, 2);
        miscLayout->addWidget(toolButtons[SAVE],         0, 3);
        miscLayout->addWidget(toolButtons[OPEN],         0, 4);
        miscLayout->addWidget(toolButtons[FULLSCREEN],   0, 5);
        miscLayout->addWidget(toolButtons[ROTATE],       0, 6);
    }
    miscLayout->addWidget(toolButtons[OVERLAYSCALEUP],     1, 0);
    miscLayout->addWidget(toolButtons[OVERLAYSCALEDOWN],   1, 1);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEUP],    1, 2);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEDOWN],  1, 3);

    toolSettingsLayout->addWidget(miscDialog);


/********** Finish him **********/


    penStyleEvent();
    penSizeEvent();
    backgroundStyleEvent();
    updateGoBackButtons();
    ov->updateImage();


}
