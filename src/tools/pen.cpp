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
    drawing->mergeSelection();
    drawing->penMode = DRAW;
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
        setPen(PEN);
    });

    penSwitch = create_button(":images/pen.svg", [=](){
        if(floatingSettings->current_page >= 0){
            floatingSettings->setHide();
            return;
        }
        drawing->penMode = DRAW;
        drawing->penStyle = SPLINE;
        drawing->lineStyle = NORMAL;
        if(drawing->penType != PEN || drawing->penMode == SELECTION){
            setPen(PEN);
        } else {
            setPen(ERASER);
        }
    });
    set_shortcut(penSwitch, Qt::Key_M, 0);

    selectButton = create_button(":images/crop.svg", [=](){
        drawing->penMode = SELECTION;
        penStyleEvent();
        penSizeEvent();
    });
    set_shortcut(selectButton, Qt::Key_X, Qt::ControlModifier);

    markerButton = create_button(":images/marker.svg", [=](){
        setPen(MARKER);
    });

    eraserButton = create_button(":images/eraser.svg", [=](){
        setPen(ERASER);
    });

    lineButton = create_button(":images/line.svg", [=](){
        setPenStyle(LINE);
    });
    set_shortcut(lineButton, Qt::Key_L, Qt::AltModifier);

    circleButton = create_button(":images/circle.svg", [=](){
        setPenStyle(CIRCLE);
    });
    set_shortcut(circleButton, Qt::Key_C, Qt::AltModifier);

    triangleButton = create_button(":images/triangle.svg", [=](){
        setPenStyle(TRIANGLE);
    });
    set_shortcut(triangleButton, Qt::Key_T, Qt::AltModifier);

    rectButton = create_button(":images/rectangle.svg", [=](){
        setPenStyle(RECTANGLE);
    });
    set_shortcut(rectButton, Qt::Key_R, Qt::AltModifier);


    splineButton = create_button(":images/spline.svg", [=](){
        setPenStyle(SPLINE);

    });
    set_shortcut(splineButton, Qt::Key_S, Qt::AltModifier);

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
    set_shortcut(colorpicker, Qt::Key_0, 0);

    backButton = create_button(":images/go-back.svg", [=](){
        drawing->goPrevious();
        updateGoBackButtons();
    });
    set_shortcut(backButton, Qt::Key_Z, Qt::ControlModifier);


    nextButton = create_button(":images/go-next.svg", [=](){
        drawing->goNext();
        updateGoBackButtons();
    });
    set_shortcut(nextButton, Qt::Key_Y, Qt::ControlModifier);

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
