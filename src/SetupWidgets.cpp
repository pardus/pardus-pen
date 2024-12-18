#include "tools/tools.h"
#include "tools/color.h"

QPushButton *penMenu;
QPushButton *toolMenu;
QWidget *colorDialog;

void setupWidgets(){
    // Pen Menu
    QWidget *penSettings = new QWidget();
    QVBoxLayout *penSettingsLayout = new QVBoxLayout(penSettings);
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

/********** Overview **********/

    penSettingsLayout->addWidget(ov);

/********** Color selection options **********/
    // color selection
    colorDialog = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(colorDialog);
    
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
            penSizeEvent();
            backgroundStyleEvent();
        });
        gridLayout->addWidget(button, (i+1) / rowsize, (i+1) % rowsize, Qt::AlignCenter);
    }
    colorDialog->setLayout(gridLayout);
    colorDialog->setFixedSize(
        butsize*rowsize + padding*(rowsize),
        butsize*(1+(num_of_color/rowsize))+ padding*((num_of_color / rowsize))
    );
    penSettingsLayout->addWidget(colorDialog);
    
    // sync overview size with colordialog
    ov->setFixedSize(
        colorDialog->size().width(),
        colorDialog->size().width()/2
    );
    
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

/********** Finish him **********/
    penStyleEvent();
    penSizeEvent();
    ov->updateImage();
}
