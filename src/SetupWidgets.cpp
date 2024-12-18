#include "tools/tools.h"
#include "tools/color.h"

QPushButton *penMenu;
QPushButton *toolMenu;
QWidget *colorDialog;

static QVBoxLayout *penSettingsLayout;

void create_seperator(){
    QLabel *hlina = new QLabel();
    hlina->setStyleSheet("background: black;");
    hlina->setFixedSize(
        colorDialog->size().width(),
        1
    );
    penSettingsLayout->addWidget(hlina);
 
}

void setupWidgets(){
    // Pen Menu
    QWidget *penSettings = new QWidget();
    penSettingsLayout = new QVBoxLayout(penSettings);
    penSettingsLayout->setSpacing(padding);
    penSettingsLayout->setContentsMargins(padding, padding, padding, padding);
    penMenu = create_button(":images/pen.svg", [=](){
           floatingSettings->setPage(0);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(penSettings);
    
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

    // Settings Menu
    QWidget *toolSettings = new QWidget();
    QVBoxLayout *toolSettingsLayout = new QVBoxLayout(toolSettings);
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
    floatingWidget->addWidget("next", nextButton);
    floatingWidget->addWidget("back", backButton);
    floatingWidget->addWidget("tool-menu", toolMenu);
    floatingWidget->addWidget("minify", minify);
    

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
    create_seperator();

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

    create_seperator();


/********** Thickness slider **********/
    
    QLabel *thicknessLabel = new QLabel(_("Size:"));
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

    create_seperator();

    
/********** penModes **********/
    QWidget *modeDialog = new QWidget();
    QGridLayout *modeLayout = new QGridLayout(modeDialog);
    // spline
    modeLayout->addWidget(splineButton, 0, 0);
    modeLayout->addWidget(lineButton, 0, 1);
    modeLayout->addWidget(circleButton, 0, 2);
    modeLayout->addWidget(triangleButton, 1, 0);
    modeLayout->addWidget(rectButton, 1, 1);
    modeLayout->addWidget(selectButton, 1,2);
    
    modeDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*2+ padding*3
    );
    penSettingsLayout->addWidget(modeDialog);

    create_seperator();

/********** penTypes **********/
    QWidget *stylDialog = new QWidget();
    QGridLayout *styleLayout = new QGridLayout(stylDialog);
    // spline
    styleLayout->addWidget(penButton, 0, 0);
    styleLayout->addWidget(eraserButton, 0, 1);
    styleLayout->addWidget(markerButton, 0, 2);
    
    stylDialog->setFixedSize(
        colorDialog->size().width(),
        butsize+ padding*2
    );
    penSettingsLayout->addWidget(stylDialog);

/********** Finish him **********/

    colorDialog->setFixedSize(
        colorDialog->size().width(),
        butsize*4+ padding*3
    );
    penStyleEvent();
    penSizeEvent();
    backgroundStyleEvent();
    ov->updateImage();
    

}
