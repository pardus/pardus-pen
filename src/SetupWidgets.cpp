#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QColorDialog>
#include <QSlider>

#include "DrawingWidget.h"
#include "FloatingWidget.h"
#include "FloatingSettings.h"
#include "WhiteBoard.h"
#include "Button.h"


extern "C" {
#include "settings.h"
}


#define padding 8

extern DrawingWidget *window;
extern FloatingWidget *floatingWidget;
extern FloatingSettings *floatingSettings;
extern WhiteBoard *board;
extern QMainWindow* mainWindow;

QPushButton *penButton;
QPushButton *markerButton;
QPushButton *eraserButton;

extern int screenWidth;
extern int screenHeight;

bool sliderLock = false;

QSlider *thicknessSlider;

QString penText = "";

static void setupPenSize(){
    QPushButton *thicknessButton = create_button(":images/close.svg",  [=](){
        floatingSettings->setPage(0);
        floatingWidget->setFloatingOffset(3);
    });
    thicknessButton->setStyleSheet(QString("background-color: none;"));

    QWidget *thickness = new QWidget();
    QVBoxLayout *thicknessLayout = new QVBoxLayout(thickness);
    thickness->setStyleSheet(
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

    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setRange(1,31);
    thicknessSlider->setSingleStep(1);
    thicknessSlider->setValue(window->penSize[PEN]);


    QLabel *thicknessLabel = new QLabel();
    thicknessLabel->setAlignment(Qt::AlignHCenter); 
    
    thicknessLayout->addWidget(thicknessLabel);
    thicknessLayout->addWidget(thicknessSlider);

    int w = screenWidth/5;
    int h = w / 4;
    
    thickness->setFixedSize(w, h);
    thicknessSlider->setFixedSize(w - padding*2, h - padding*2 - h /8);
    thicknessLabel->setFixedSize(w - padding*2, h /8);
    thickness->show();

    QObject::connect(thicknessSlider, &QSlider::valueChanged, [thicknessLabel](int value) {
        switch(window->penType){
            case PEN:
                penText = "Pen";
                set_int((char*)"pen-size",value);
                break;
            case MARKER:
                penText = "Marker";
                set_int((char*)"marker-size",value);
                break;
            case ERASER:
                penText = "Eraser";
                set_int((char*)"eraser-size",value);
                break;
        }
        thicknessLabel->setText(QString(penText)+QString(" Size: ")+QString::number(value));
        if(!sliderLock){
            window->penSize[window->penType] = value;
        }
        
    });
    thicknessLabel->setText(QString("Pen")+QString(" Size: ")+QString::number(window->penSize[PEN]));
    floatingSettings->addPage(thickness);
    floatingWidget->setWidget(thicknessButton);


}

static void penStyleEvent(){
    if(window->penType == PEN){
        penButton->setStyleSheet("background-color:"+window->penColor.name()+";");
        markerButton->setStyleSheet(QString("background-color: none;"));
        eraserButton->setStyleSheet(QString("background-color: none;"));
    } else if(window->penType == MARKER){
        penButton->setStyleSheet(QString("background-color: none;"));
        markerButton->setStyleSheet("background-color:"+window->penColor.name()+";");
        eraserButton->setStyleSheet(QString("background-color: none;"));
    } else{
        penButton->setStyleSheet(QString("background-color: none;"));
        markerButton->setStyleSheet(QString("background-color: none;"));
        eraserButton->setStyleSheet("background-color:"+window->penColor.name()+";");
    }
}


static void setupPenColor(){
    QWidget *colorWidget = new QWidget();
    QVBoxLayout *colorLayout = new QVBoxLayout(colorWidget);
    QLabel *colorLabel = new QLabel();
    colorLabel->setText(QString(penText)+QString(" Color:"));
    colorLabel->setAlignment(Qt::AlignHCenter); 

    QPushButton *colorButton = create_button(":images/clear.svg",  [=](){
        floatingSettings->setPage(1);
        floatingWidget->setFloatingOffset(4);
    });

    colorButton->setStyleSheet(QString("background-color: none;"));
    colorWidget->setStyleSheet(QString("background-color: none;"));

    QWidget *colorDialog = new QWidget();    
    colorDialog->setWindowTitle("Color Picker");

    QGridLayout *gridLayout = new QGridLayout(colorDialog);

    // Define colors
    QColor colors[] = {
        QColor(255, 0, 0),    // Red
        QColor(0, 255, 0),    // Green
        QColor(0, 0, 255),    // Blue
        QColor(255, 255, 0),  // Yellow
        QColor(255, 0, 255),  // Magenta
        QColor(0, 255, 255),  // Cyan
        QColor(255, 255, 255) // White
    };


    // Create buttons for each color
    int num_of_color = sizeof(colors) / sizeof(QColor);
    int rowsize = 4;
    int butsize = screenHeight/23;
    for (int i = 0; i < num_of_color; i++) {
        QPushButton *button = new QPushButton(colorDialog);
        button->setFixedSize(butsize, butsize);
        button->setStyleSheet(QString(
             "background-color: %1;"
             "border-radius: 12px;"
        ).arg(colors[i].name()));
        QObject::connect(button, &QPushButton::clicked, [=]() {
            window->penColor = colors[i];
            penStyleEvent();
            if(window->penType == ERASER) {
                sliderLock = true;
                window->penColor.setAlpha(255);
                window->penType = PEN;
                thicknessSlider->setRange(1,31);
                thicknessSlider->setValue(window->penSize[PEN]);
                sliderLock = false;
            }
        });
        gridLayout->addWidget(button, i / rowsize, i % rowsize, Qt::AlignCenter);
    }

    colorDialog->setLayout(gridLayout);
    colorDialog->setFixedSize(
        butsize*rowsize + padding*(rowsize),
        butsize*(1+(num_of_color/rowsize))+ padding*((num_of_color / rowsize)+2)
    );

    colorLayout->addWidget(colorLabel);
    colorLayout->addWidget(colorDialog);
    
    colorWidget->setFixedSize(
        colorDialog->size().width(),
        butsize
    );
    
    colorWidget->setFixedSize(
       colorDialog->size().width() + padding*2,
       colorDialog->size().height() + butsize + padding*2
    );
    colorWidget->show();
    
    floatingSettings->addPage(colorWidget);
    floatingWidget->setWidget(colorButton);
}

static void setupPenType(){

    penButton = create_button(":images/pen.svg", [=](){
        sliderLock = true;
        window->penColor.setAlpha(255);
        window->penType = PEN;
        penStyleEvent();
        thicknessSlider->setRange(1,31);
        thicknessSlider->setValue(window->penSize[PEN]);
        sliderLock = false;
    });
    floatingWidget->setWidget(penButton);

    markerButton = create_button(":images/marker.svg", [=](){
        sliderLock = true;
        window->penColor.setAlpha(127);
        window->penType = MARKER;
        penStyleEvent();
        thicknessSlider->setRange(1,100);
        thicknessSlider->setValue(window->penSize[MARKER]);
        sliderLock = false;
    });
    floatingWidget->setWidget(markerButton);

    eraserButton = create_button(":images/eraser.svg", [=](){
        sliderLock = true;
        window->penColor.setAlpha(255);
        window->penType = ERASER;
        penStyleEvent();
        thicknessSlider->setRange(1,310);
        thicknessSlider->setValue(window->penSize[ERASER]);
        sliderLock = false;
    });
    floatingWidget->setWidget(eraserButton);
    penStyleEvent();
}

#define addToBackgroundWidget(A) \
    if(A->size().height() > h){ \
        h = A->size().height() + padding*2; \
    } \
    w += A->size().width(); \
    backgroundLayout->addWidget(A);

static void setupBackground(){
    int w = padding*2;
    int h = padding*2;
    QPushButton *backgroundButton = create_button(":images/clear.svg",  [=](){
        floatingSettings->setPage(2);
        floatingWidget->setFloatingOffset(5);
    });
    QWidget *backgroundWidget = new QWidget();
    QHBoxLayout *backgroundLayout = new QHBoxLayout(backgroundWidget);
    backgroundLayout->setSpacing(padding);
    QPushButton* transparent = create_button(":images/paper-transparent.svg", [=](){
        board->disable();
    });
    QPushButton* black = create_button(":images/paper-black.svg", [=](){
        board->enableDark();
    });
    QPushButton* white = create_button(":images/paper-white.svg", [=](){
        board->enable();
    });
    addToBackgroundWidget(transparent);
    addToBackgroundWidget(white);
    addToBackgroundWidget(black);

    backgroundWidget->setFixedSize(w + padding*2,h);

    backgroundWidget->setStyleSheet(QString("background-color: none;"));
    backgroundButton->setStyleSheet(QString("background-color: none;"));

    floatingSettings->addPage(backgroundWidget);
    floatingWidget->setWidget(backgroundButton);
}

void setupWidgets(){
    window->floatingSettings = floatingSettings;
    setupPenType();
    setupPenSize();
    setupPenColor();
    setupBackground();
    
}
