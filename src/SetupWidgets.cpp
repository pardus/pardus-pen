#include "tools/tools.h"
#include "tools/color.h"

QPushButton *penMenu;
QPushButton *toolMenu;
QWidget *colorDialog;

void setupWidgets(){
    // Pen Menu
    QWidget *penSettings = new QWidget();
    QVBoxLayout *penSettingsLayout = new QVBoxLayout(penSettings);
    penSettingsLayout->setSpacing(0);
    penSettingsLayout->setContentsMargins(0, 0, 0, 0);
    penMenu = create_button(":images/pen.svg", [=](){
           floatingSettings->setPage(0);
           floatingWidget->moveAction();
    });
    floatingSettings->addPage(penSettings);

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
    
    // finish him
    penStyleEvent();
    penSizeEvent();


}
