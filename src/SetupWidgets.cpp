#include "tools.h"
#include "tools/color.h"

#include <dirent.h>

QWidget *colorDialog;

QLabel *thicknessLabel;

QWidget *penTypeDialog;
QWidget *modeDialog;

QWidget *bgMenu;


static QVBoxLayout *penSettingsLayout;
static QVBoxLayout *pageSettingsLayout;
static QVBoxLayout *utilSettingsLayout;

void setupWidgets(){
    // Pen Settings Menu
    QWidget *penSettings = new QWidget();
    penSettingsLayout = new QVBoxLayout(penSettings);
    penSettingsLayout->setSpacing(padding);
    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penSettings->setStyleSheet(QString("background-color: none;"));
    floatingSettings->addPage(penSettings);


    // Pen button with menu
    toolButtons[PENMENU] = create_button(PEN, [=](){
        if(floatingSettings->current_page == 0 && drawing->getPen() != ERASER){
            floatingSettings->setHide();
            return;
        }
        if(drawing->getPen() != PEN || drawing->getPenStyle() != SPLINE){
            setPen(PEN);
            setPenStyle(SPLINE);
            floatingSettings->setHide();
            return;
        }
        floatingSettings->setPage(0);
        floatingWidget->moveAction();
    });

    // Eraser button with menu
    toolButtons[ERASERMENU] = create_button(ERASER, [=](){
        if(floatingSettings->current_page == 0 && drawing->getPen() == ERASER){
            floatingSettings->setHide();
            return;
        }
        if(drawing->getPen() != ERASER){
            setPen(ERASER);
            setPenStyle(SPLINE);
            floatingSettings->setHide();
            return;
        }
        floatingSettings->setPage(0);
        floatingWidget->moveAction();
    });

    toolButtons[SHAPEMENU] = create_button(SPLINE, [=](){
        if(floatingSettings->current_page == 0){
            floatingSettings->setHide();
            return;
        }
        if(drawing->getPen() != PEN && drawing->getPen() != MARKER){
            setPen(PEN);
        }
	      floatingSettings->setPage(0);
        floatingWidget->moveAction();
    });

    // Page settings menu
    QWidget *pageSettings = new QWidget();
    pageSettings->setStyleSheet(QString("background-color: none;"));
    pageSettingsLayout = new QVBoxLayout(pageSettings);
    pageSettingsLayout->setSpacing(padding);
    pageSettingsLayout->setContentsMargins(0, 0, 0, 0);
    floatingSettings->addPage(pageSettings);

    // Page menu button
    toolButtons[PAGEMENU] = create_button(PAGEMENU, [=](){
        if(floatingSettings->current_page == 1){
            floatingSettings->setHide();
            return;
        }
        floatingSettings->setPage(1);
        floatingWidget->moveAction();
    });

    // Page menu button
    toolButtons[UTILMENU] = create_button(UTILMENU, [=](){
        if(floatingSettings->current_page == 2){
            floatingSettings->setHide();
            return;
        }
        floatingSettings->setPage(2);
        floatingWidget->moveAction();
    });


    floatingSettings->setHide();


/********** Main toolbar **********/

    floatingWidget->addWidget(toolButtons[PENMENU]);
    floatingWidget->addWidget(toolButtons[ERASERMENU]);
    floatingWidget->addWidget(toolButtons[SHAPEMENU]);
    floatingWidget->addWidget(penButtons[SELECTION]);
    floatingWidget->addWidget(toolButtons[MINIFY]);
    floatingWidget->addWidget(toolButtons[CLEAR]);
    floatingWidget->addWidget(toolButtons[BACK]);
    floatingWidget->addWidget(toolButtons[NEXT]);
    floatingWidget->addWidget(toolButtons[PAGEMENU]);
    floatingWidget->addWidget(toolButtons[UTILMENU]);
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

    penSettingsLayout->addWidget(ov, Qt::AlignCenter);


/********** Thickness slider **********/

    QWidget *penSizeSettings = new QWidget();
    QVBoxLayout *penSizeSettingsLayout = new QVBoxLayout(penSizeSettings);
    penSettingsLayout->addWidget(penSizeSettings, Qt::AlignCenter);


    penSizeSettings->setStyleSheet(
    "QWidget {"
    "background-color: #f3232323;"
    "}"
    );


    thicknessLabel = new QLabel(_("Size: 10"));
    thicknessLabel->setStyleSheet(
        "background: none;"
        "color: #9e9e9e;"
        "padding-left: "+QString::number(8*scale)+"px;"
        "padding-top: "+QString::number(4*scale)+"px;"
    );
    thicknessLabel->setAlignment(Qt::AlignLeft);
    penSizeSettingsLayout->addWidget(thicknessLabel, Qt::AlignLeft);

    //thicknessLabel->setFixedSize(
    //    colorDialog->size().width(),
    //    butsize / 2
    //);

    thicknessSlider->setFixedSize(
        colorDialog->size().width() - 2*padding,
        butsize
    );
    penSizeSettingsLayout->addWidget(thicknessSlider, Qt::AlignHCenter | Qt::AlignVCenter);

    thicknessSlider->setStyleSheet(
         "QWidget {"
         "background: none;"
         "}"
         "QSlider::groove:horizontal {"
            "border: 1px solid #bbb;"
            "background: white;"
            "height: "+QString::number(16*scale)+"px;"
            "border-radius: "+QString::number(8*scale)+"px;"
        "}"
        "QSlider::handle:horizontal {"
            "background: #fff;"
            "width: "+QString::number(44*scale)+"px;"
            "border-radius: "+QString::number(8*scale)+"px;"
            "margin: -4px 0;"
        "}"
        "QSlider::handle:horizontal:hover {"
            "background: #ccc;"
        "}"
        "QSlider::sub-page:horizontal {"
            "background: #4a90e2;"
            "border-radius: 5px;"
        "}"
        "QSlider::add-page:horizontal {"
            "background: #4a4a4a;"
            "border-radius:5px;"
        "}"
    );


/********** penTypes **********/
    QWidget *penTypeMainWidget = new QWidget();
    QHBoxLayout *penTypeMainLayout = new QHBoxLayout(penTypeMainWidget);
    penTypeMainWidget->setStyleSheet("background: none;");

    QWidget *styleDialog = new QWidget();
    QGridLayout *styleLayout = new QGridLayout(styleDialog);

    styleLayout->addWidget(penButtons[PEN],           0, 0, Qt::AlignCenter);
    styleLayout->addWidget(penButtons[MARKER],        0, 1, Qt::AlignCenter);
    styleLayout->addWidget(penButtons[ERASER],        0, 2, Qt::AlignCenter);
    //styleLayout->addWidget(toolButtons[CLEAR],        0, 3, Qt::AlignCenter);

    penSettingsLayout->addWidget(styleDialog, Qt::AlignCenter);

    penTypeDialog = new QWidget();
    QGridLayout *penTypeLayout = new QGridLayout(penTypeDialog);
    penTypeLayout->addWidget(penButtons[NORMAL],   0, 0, Qt::AlignCenter);
    penTypeLayout->addWidget(penButtons[DOTLINE],  0, 1, Qt::AlignCenter);
    penTypeLayout->addWidget(penButtons[LINELINE], 0, 2, Qt::AlignCenter);


    penTypeMainLayout->addWidget(styleDialog);
    penTypeMainLayout->addWidget(penTypeDialog);

    penSizeSettingsLayout->addWidget(penTypeMainWidget, Qt::AlignCenter);

/********** penModes **********/
    modeDialog = new QWidget();
    QGridLayout *modeLayout = new QGridLayout(modeDialog);
    // spline
    modeLayout->addWidget(penButtons[SPLINE],     0, 0);
    modeLayout->addWidget(penButtons[LINE],       0, 1);
    modeLayout->addWidget(penButtons[CIRCLE],     0, 2);
    modeLayout->addWidget(penButtons[TRIANGLE],   0, 3);
    modeLayout->addWidget(penButtons[RECTANGLE],  0, 4);
    modeLayout->addWidget(penButtons[VECTOR],     0, 5);
    modeLayout->addWidget(penButtons[VECTOR2],    0, 6);

    modeDialog->setStyleSheet("background-color: #f4141414;");

    penSizeSettingsLayout->addWidget(modeDialog, Qt::AlignCenter);


/********** Color selection options **********/
    // color selection

    colorDialog->setStyleSheet(
    "QWidget {"
    "background-color: #f3232323;"
    "}"
    );


    QGridLayout *gridLayout = new QGridLayout(colorDialog);

    // Create buttons for each color
    gridLayout->addWidget(toolButtons[COLORPICKER], 0, 0, Qt::AlignCenter);

    // Color button offset is 100
    for (int i = 0; i < num_of_color; i++) {
        toolButtons[i+100] = create_color_button(colors[i]);
        gridLayout->addWidget(toolButtons[i+100], (i+1) / rowsize, (i+1) % rowsize, Qt::AlignCenter);
    }
    colorDialog->setLayout(gridLayout);
    penSettingsLayout->addWidget(colorDialog, Qt::AlignCenter);

    // sync overview size with colordialog
    ov->setFixedSize(
        colorDialog->size().width(),
        colorDialog->size().width()/2
    );
    // sync all other widget widths
    penSizeSettings->setFixedWidth(colorDialog->size().width());


/********** pen type **********/

    // resize color dialog
    colorDialog->setFixedHeight(
        butsize*3+ padding*4
    );


/************ Page Menu ************/

 /********** page number **********/

    QWidget *pageMenu = new QWidget();
    QVBoxLayout *pageMenuLayout = new QVBoxLayout(pageMenu);

    pageMenu->setStyleSheet(
        "QWidget {"
          "background-color: #f3232323;"
        "}"
    );


    QWidget *pageNumWidget = new QWidget();
    QHBoxLayout *pageNumLayout = new QHBoxLayout(pageNumWidget);


    pageNumWidget->setStyleSheet("background: none;");

    QLabel* pgLabel = new QLabel(_("Page:"));
    pgLabel->setStyleSheet(
        "background: none;"
        "color: #c0c0c0;"
        "padding-left: "+QString::number(8*scale)+"px;"
        "padding-top: "+QString::number(4*scale)+"px;"
    );

    pageMenuLayout->addWidget(pgLabel);

    pageNumLayout->addWidget(toolButtons[PREVPAGE], Qt::AlignLeft);
    pageNumLayout->addWidget(pageLabel, Qt::AlignLeft);
    pageNumLayout->addWidget(toolButtons[NEXTPAGE], Qt::AlignLeft);

    pageLabel->setFixedSize(butsize, butsize);

    pageLabel->setStyleSheet(
        "background: none;"
        "color: #c0c0c0;"
    );



    pageNumLayout->addWidget(penButtons[TRANSPARENT], Qt::AlignRight);
    pageNumLayout->addWidget(penButtons[BLACK], Qt::AlignRight);
    pageNumLayout->addWidget(penButtons[WHITE], Qt::AlignRight);

    pageMenuLayout->addWidget(pageNumWidget);
    pageSettingsLayout->addWidget(pageMenu);

/********** page type **********/

    bgMenu = new QWidget();
    QVBoxLayout *bgMenuLayout = new QVBoxLayout(bgMenu);

    bgMenu->setStyleSheet(
        "QWidget {"
          "background-color: #f3232323;"
        "}"
    );



    QLabel *bgLabel = new QLabel(_("Background:"));
    bgLabel->setStyleSheet(
        "background: none;"
        "color: #c0c0c0;"
        "padding-left: "+QString::number(8*scale)+"px;"
        "padding-top: "+QString::number(4*scale)+"px;"
    );
    bgLabel->setAlignment(Qt::AlignLeft);
    bgMenuLayout->addWidget(bgLabel);

    QWidget *pageDialog = new QWidget();
    QGridLayout *pageLayout = new QGridLayout(pageDialog);

    pageDialog->setStyleSheet(
        "background: none;"
        "color: #c0c0c0"
    );


    // spline
    pageLayout->addWidget(penButtons[BLANK],     0, 0, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[SQUARES],   0, 1, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[LINES],     0, 2, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[ISOMETRIC], 0, 3, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[MUSIC],     0, 4, Qt::AlignCenter);
    pageLayout->addWidget(penButtons[CUSTOM],    0, 5, Qt::AlignCenter);

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
            toolButtons[i+200] = create_button(0, [=](){
                drawing->setOverlay(QImage(path), drawing->getPageNum());
                board->setOverlayType(CUSTOM);
                board->rotates[drawing->getPageNum()] = 0;
                board->ratios[drawing->getPageNum()] = 100;
                board->updateTransform();
            });
            set_icon(path.toStdString().c_str(), toolButtons[i+200]);
            pageLayout->addWidget(toolButtons[i+200], i / 6, i % 6, Qt::AlignCenter);
            i++;
            //printf ("%s\n", ep->d_name);
        }
        closedir(dp);
    }
    pageDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*2+ padding*3
    );
    bgMenuLayout->addWidget(pageDialog);

    pageSettingsLayout->addWidget(bgMenu);


/********** clear & screenshot **********/

    QWidget *miscDialog = new QWidget();

    miscDialog->setStyleSheet(
    "QWidget {"
    "background-color: #f3232323;"
    "}"
    );

    QGridLayout *miscLayout = new QGridLayout(miscDialog);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEUP],    0, 0, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYSCALEUP],     0, 1, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYSCALEDOWN],   0, 2, Qt::AlignCenter);
    miscLayout->addWidget(toolButtons[OVERLAYROTATEDOWN],  0, 3, Qt::AlignCenter);

    pageSettingsLayout->addWidget(miscDialog);

/********** Util Settings **********/

    QWidget *utilSettings = new QWidget();

    utilSettings->setStyleSheet(
    "QWidget {"
    "background-color: #f3232323;"
    "}"
    );


    utilSettingsLayout = new QVBoxLayout(utilSettings);
    floatingSettings->addPage(utilSettings);


    utilSettingsLayout->addWidget(toolButtons[SAVE],  Qt::AlignCenter);
    utilSettingsLayout->addWidget(toolButtons[OPEN],  Qt::AlignCenter);
    utilSettingsLayout->addWidget(toolButtons[CLOSE],  Qt::AlignCenter);
    utilSettingsLayout->addWidget(toolButtons[SCREENSHOT],  Qt::AlignCenter);
    utilSettingsLayout->addWidget(toolButtons[FULLSCREEN],  Qt::AlignCenter);
    utilSettingsLayout->addWidget(toolButtons[ROTATE],  Qt::AlignCenter);

    if(get_bool("fuar")){
        toolButtons[SAVE]->setEnabled(false);
        toolButtons[OPEN]->setEnabled(false);
        toolButtons[CLOSE]->setEnabled(false);
        toolButtons[SAVE]->setEnabled(false);
    }

/********** Finish him **********/
    updateGui();
    pageLabel->setStyleSheet(
    "color: #c0c0c0;"
    "font-size: "+QString::number(31*scale)+"px;"
    );
}
