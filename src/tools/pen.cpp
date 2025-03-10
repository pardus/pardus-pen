#include "../tools.h"

QPushButton **penButtons;

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
    drawing->setPen(type);
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
    thicknessSlider->setValue(drawing->penSize[type]);
    sliderLock = false;
    if(pen_init){
        penStyleEvent();
        penSizeEvent();
    }
}

int getPen(){
    return drawing->getPen();
}

void setPenStyle(int style){
    drawing->setPenStyle(style);
    penStyleEvent();
    penSizeEvent();
}

void setLineStyle(int style){
    drawing->setLineStyle(style);
    penStyleEvent();
    penSizeEvent();
}

void setupPenType(){
    penButtons = (QPushButton**)calloc(1024, sizeof(QPushButton*));

    penButtons[PEN] = create_button(":images/pen.svg", [=](){
        setPen(PEN);
    });

    penSwitch = create_button(":images/pen.svg", [=](){
        if(floatingSettings->current_page >= 0){
            floatingSettings->setHide();
            return;
        }
        drawing->setPenStyle(SPLINE);
        drawing->setLineStyle(NORMAL);
        if(getPen() != PEN){
            setPen(PEN);
        } else {
            setPen(ERASER);
        }
    });
    set_shortcut(penSwitch, Qt::Key_M, 0);

    penButtons[SELECTION] = create_button(":images/crop.svg", [=](){
        setPen(SELECTION);
    });
    set_shortcut(penButtons[SELECTION], Qt::Key_X, Qt::ControlModifier);

    penButtons[MARKER] = create_button(":images/marker.svg", [=](){
        setPen(MARKER);
    });

    penButtons[ERASER] = create_button(":images/eraser.svg", [=](){
        setPen(ERASER);
    });

    penButtons[LINE] = create_button(":images/line.svg", [=](){
        setPenStyle(LINE);
    });

    penButtons[VECTOR] = create_button(":images/vector.svg", [=](){
        setPenStyle(VECTOR);
    });
    set_shortcut(penButtons[VECTOR], Qt::Key_V, Qt::AltModifier);

    penButtons[VECTOR2] = create_button(":images/vector2.svg", [=](){
        setPenStyle(VECTOR2);
    });
    set_shortcut(penButtons[VECTOR2], Qt::Key_W, Qt::AltModifier);

    penButtons[CIRCLE] = create_button(":images/circle.svg", [=](){
        setPenStyle(CIRCLE);
    });
    set_shortcut(penButtons[CIRCLE], Qt::Key_C, Qt::AltModifier);

    penButtons[TRIANGLE] = create_button(":images/triangle.svg", [=](){
        setPenStyle(TRIANGLE);
    });
    set_shortcut(penButtons[TRIANGLE], Qt::Key_T, Qt::AltModifier);

    penButtons[RECTANGLE] = create_button(":images/rectangle.svg", [=](){
        setPenStyle(RECTANGLE);
    });
    set_shortcut(penButtons[TRIANGLE], Qt::Key_R, Qt::AltModifier);


    penButtons[SPLINE] = create_button(":images/spline.svg", [=](){
        setPenStyle(SPLINE);

    });
    set_shortcut(penButtons[SPLINE], Qt::Key_S, Qt::AltModifier);

    penButtons[NORMAL] = create_button(":images/line-normal.svg", [=](){
        setLineStyle(NORMAL);

    });

    penButtons[DOTLINE] = create_button(":images/line-dotline.svg", [=](){
        setLineStyle(DOTLINE);

    });

    penButtons[LINELINE] = create_button(":images/line-lineline.svg", [=](){
        setLineStyle(LINELINE);

    });

    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setSingleStep(1);
    setPen(PEN);

    QObject::connect(thicknessSlider, &QSlider::valueChanged, [=](int value) {
        if(!sliderLock){
            drawing->penSize[getPen()] = value;
        }
        penSizeEvent();
    });
    
    QObject::connect(thicknessSlider, &QSlider::sliderReleased, [=]() {
        int value = drawing->penSize[getPen()];
        switch(getPen()){
            case PEN:
                set_int((char*)"pen-size",value);
                break;
            case MARKER:
                set_int((char*)"marker-size",value);
                break;
            case ERASER:
                set_int((char*)"eraser-size",value);
                break;
         }
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
