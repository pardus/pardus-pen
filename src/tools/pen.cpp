#include "../tools.h"

QPushButton *penButton;
QPushButton *selectButton;
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

QPushButton *penSwitch;

QSlider *thicknessSlider;
QPushButton *backgroundButton;
QPushButton* minify;
QPushButton* rotate;
QPushButton* fullscreen;

QPushButton *backButton;
QPushButton *nextButton;

QPushButton *previousPage;
QPushButton *nextPage;

QLabel *move;
QPushButton *close;
QPushButton* colorpicker;

OverView *ov;

static bool sliderLock = false;
static bool pen_init = false;
void setPen(int type){
    sliderLock = true;
    drawing->penType = type;
    switch(type){
        case ERASER:
            thicknessSlider->setRange(10*scale,200*scale);
            break;
        case MARKER:
            thicknessSlider->setRange(1,50*scale);
            break;
        case PEN:
            thicknessSlider->setRange(1,50*scale);
            break;
    
    }
    thicknessSlider->setValue(drawing->penSize[type]);
    sliderLock = false;
    if(pen_init){
        penStyleEvent();
        penSizeEvent();
    }
}

void setPenStyle(int style){
    drawing->penStyle = style;
    penStyleEvent();
    penSizeEvent();
}

void setLineStyle(int style){
    drawing->lineStyle = style;
    penStyleEvent();
    penSizeEvent();
}

void setupPenType(){

    penButton = create_button(":images/pen.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        setPen(PEN);
    });

    penSwitch = create_button(":images/pen.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        if(drawing->penType == ERASER){
            setPen(PEN);
        } else {
            setPen(ERASER);
        }
        floatingSettings->setHide();
    });

    selectButton = create_button(":images/crop.svg", [=](){
        drawing->penMode = SELECTION;
        penStyleEvent();
        penSizeEvent();
    });

    markerButton = create_button(":images/marker.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        setPen(MARKER);
    });

    eraserButton = create_button(":images/eraser.svg", [=](){
        drawing->mergeSelection();
        drawing->penMode = DRAW;
        setPen(ERASER);
    });

    lineButton = create_button(":images/line.svg", [=](){
        setPenStyle(LINE);
    });

    circleButton = create_button(":images/circle.svg", [=](){
        setPenStyle(CIRCLE);
    });

    triangleButton = create_button(":images/triangle.svg", [=](){
        setPenStyle(TRIANGLE);
    });

    rectButton = create_button(":images/rectangle.svg", [=](){
        setPenStyle(RECTANGLE);
    });

    splineButton = create_button(":images/spline.svg", [=](){
        setPenStyle(SPLINE);

    });

    lineNormalButton = create_button(":images/line-normal.svg", [=](){
        setLineStyle(NORMAL);

    });

    lineDotLineButton = create_button(":images/line-dotline.svg", [=](){
        setLineStyle(DOTLINE);

    });

    lineLineLineButton = create_button(":images/line-lineline.svg", [=](){
        setLineStyle(LINELINE);

    });
    
    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setSingleStep(1);
    setPen(PEN);
    
    QObject::connect(thicknessSlider, &QSlider::valueChanged, [=](int value) {
        if(!sliderLock){
            drawing->penSize[drawing->penType] = value;
        }
        penSizeEvent();
    });
    
    colorpicker = create_button(":images/color-picker.svg", [=](){
        floatingWidget->hide();
        floatingSettings->setHide();
        setHideMainWindow(true);
        QColor newCol = QColorDialog::getColor(drawing->penColor, NULL, _("Select Color"));
        setHideMainWindow(false);
        floatingWidget->show();
        if(! newCol.isValid()){
            return;
        }
        drawing->penColor = newCol;
        set_string((char*)"color", (char*)drawing->penColor.name().toStdString().c_str());
        penStyleEvent();
        penSizeEvent();
        backgroundStyleEvent();
    });
    
    backButton = create_button(":images/go-back.svg", [=](){
        drawing->goPrevious();
        updateGoBackButtons();
    });
    
    nextButton = create_button(":images/go-next.svg", [=](){
        drawing->goNext();
        updateGoBackButtons();
    });
    
    close = create_button(":images/close.svg", [=](){
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
    
    move = new QLabel("");
    QIcon icon = QIcon(":images/move-icon.svg");
    QPixmap pixmap = icon.pixmap(icon.actualSize(QSize(butsize, butsize)));
    move->setPixmap(pixmap);
    move->setStyleSheet(QString("background-color: none;"));
    move->setFixedSize(butsize, butsize);
    pen_init = true;
}
