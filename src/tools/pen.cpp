#include "../tools.h"

QMap<qint64, QPushButton*> penButtons;
QMap<qint64, QPushButton*> toolButtons;

QSlider *thicknessSlider;


OverView *ov;

static bool sliderLock = false;
static bool pen_init = false;
void setPen(int type){
    sliderLock = true;
    drawing->setPen(type);


    switch(type){
        case ERASER:
            penButtons[ERASER]->hide();
            penButtons[MARKER]->show();
            penButtons[PEN]->show();
            toolButtons[CLEAR]->show();
            thicknessSlider->setRange(10*scale,200*scale);
            break;
        case MARKER:
            penButtons[MARKER]->hide();
            toolButtons[CLEAR]->show();
            penButtons[ERASER]->show();
            penButtons[PEN]->show();
            thicknessSlider->setRange(1,50*scale);
            break;
        case PEN:
            penButtons[PEN]->hide();
            toolButtons[CLEAR]->show();
            penButtons[ERASER]->show();
            penButtons[MARKER]->show();
            thicknessSlider->setRange(1,50*scale);
            break;

    }
    drawing->mergeSelection();
    thicknessSlider->setValue(drawing->penSize[type]);
    sliderLock = false;
    if(pen_init){
        updateGui();
    }
}

int getPen(){
    return drawing->getPen();
}

void setPenStyle(int style){
    drawing->setPenStyle(style);
}

void setLineStyle(int style){
    drawing->setLineStyle(style);
}

void setupPenType(){
    penButtons[PEN] = create_button(PEN, [=](){
        setPen(PEN);
    });

    penButtons[SELECTION] = create_button(SELECTION, [=](){
        setPen(SELECTION);
        floatingSettings->setHide();
    });
    set_shortcut(penButtons[SELECTION], Qt::Key_X, Qt::ControlModifier);

    penButtons[MARKER] = create_button(MARKER, [=](){
        setPen(MARKER);
    });

    penButtons[ERASER] = create_button(ERASER, [=](){
        setPenStyle(SPLINE);
        setPen(ERASER);
    });

    penButtons[LINE] = create_button(LINE, [=](){
        setPenStyle(LINE);
    });

    penButtons[VECTOR] = create_button(VECTOR, [=](){
        setPenStyle(VECTOR);
    });
    set_shortcut(penButtons[VECTOR], Qt::Key_V, Qt::AltModifier);

    penButtons[VECTOR2] = create_button(VECTOR2, [=](){
        setPenStyle(VECTOR2);
    });
    set_shortcut(penButtons[VECTOR2], Qt::Key_W, Qt::AltModifier);

    penButtons[CIRCLE] = create_button(CIRCLE, [=](){
        setPenStyle(CIRCLE);
    });
    set_shortcut(penButtons[CIRCLE], Qt::Key_C, Qt::AltModifier);

    penButtons[TRIANGLE] = create_button(TRIANGLE, [=](){
        setPenStyle(TRIANGLE);
    });
    set_shortcut(penButtons[TRIANGLE], Qt::Key_T, Qt::AltModifier);

    penButtons[RECTANGLE] = create_button(RECTANGLE, [=](){
        setPenStyle(RECTANGLE);
    });
    set_shortcut(penButtons[TRIANGLE], Qt::Key_R, Qt::AltModifier);


    penButtons[SPLINE] = create_button(SPLINE, [=](){
        setPenStyle(SPLINE);

    });
    set_shortcut(penButtons[SPLINE], Qt::Key_S, Qt::AltModifier);

    penButtons[NORMAL] = create_button(NORMAL, [=](){
        setLineStyle(NORMAL);

    });

    penButtons[DOTLINE] = create_button(DOTLINE, [=](){
        setLineStyle(DOTLINE);

    });

    penButtons[LINELINE] = create_button(LINELINE, [=](){
        setLineStyle(LINELINE);

    });

    thicknessSlider = new QSlider(Qt::Horizontal);
    thicknessSlider->setSingleStep(1);

    QObject::connect(thicknessSlider, &QSlider::valueChanged, [=](int value) {
        if(!sliderLock){
            drawing->penSize[getPen()] = value;
        }
        thicknessLabel->setText(QString(_("Size:"))+QString(" ")+QString::number(value));
        ov->updateImage();
    });

    QObject::connect(thicknessSlider, &QSlider::sliderReleased, [=]() {
        int value = drawing->penSize[getPen()];
        switch(getPen()){
            case PEN:
                set_int("pen-size",value);
                break;
            case MARKER:
                set_int("marker-size",value);
                break;
            case ERASER:
                set_int("eraser-size",value);
                break;
         }
    });

    toolButtons[COLORPICKER] = create_button(COLORPICKER, [=](){
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
        set_string("color", drawing->penColor.name());
    });
    set_shortcut(toolButtons[COLORPICKER], Qt::Key_0, 0);

    toolButtons[BACK] = create_button(BACK, [=](){
        drawing->goPrevious();
    });
    set_shortcut(toolButtons[BACK], Qt::Key_Z, Qt::ControlModifier);


    toolButtons[NEXT] = create_button(NEXT, [=](){
        drawing->goNext();
    });
    set_shortcut(toolButtons[NEXT], Qt::Key_Y, Qt::ControlModifier);

    toolButtons[CLOSE] = create_button(CLOSE, [=](){
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
        exit(0);
    });

    pen_init = true;
}
