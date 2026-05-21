#include <constants.h>
#include <widgets/DrawingWidget.h>
#include <widgets/Button.h>
#include <widgets/FloatingSettings.h>
#include <widgets/WhiteBoard.h>
#include <widgets/FloatingWidget.h>
#include <widgets/Background.h>
#include <widgets/OverView.h>
#include <utils/Settings.h>
#include <utils/eta-keyboard.h>

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
            penButtons[PENTEXT]->show();
            thicknessSlider->setRange(10*scale,200*scale);
            break;
        case MARKER:
            penButtons[MARKER]->hide();
            penButtons[PENTEXT]->show();
            penButtons[ERASER]->show();
            penButtons[PEN]->show();
            thicknessSlider->setRange(1,50*scale);
            break;
        case PEN:
            penButtons[PEN]->hide();
            penButtons[PENTEXT]->show();
            penButtons[ERASER]->show();
            penButtons[MARKER]->show();
            thicknessSlider->setRange(1,50*scale);
            break;
        case PENTEXT:
            penButtons[PENTEXT]->hide();
            penButtons[PEN]->show();
            penButtons[MARKER]->show();
            penButtons[ERASER]->show();
            thicknessSlider->setRange(8,200*scale);
            showEtaKeyboard();
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
    int penTypes[] = {PEN, MARKER, PENTEXT};
    for (int t : penTypes) {
        penButtons[t] = create_button(t, [=](){ setPen(t); });
    }
    set_shortcut(penButtons[PENTEXT], Qt::Key_T, Qt::ControlModifier);

    penButtons[SELECTION] = create_button(SELECTION, [=](){
        setPen(SELECTION);
        floatingSettings->setHide();
    });
    set_shortcut(penButtons[SELECTION], Qt::Key_X, Qt::ControlModifier);

    penButtons[ERASER] = create_button(ERASER, [=](){
        setPenStyle(SPLINE);
        setPen(ERASER);
    });

    int penStyles[] = {LINE, VECTOR, VECTOR2, CIRCLE, TRIANGLE, RECTANGLE, SPLINE};
    for (int s : penStyles) {
        penButtons[s] = create_button(s, [=](){
           setPenStyle(s);
        });
    }
    set_shortcut(penButtons[VECTOR],    Qt::Key_V, Qt::AltModifier);
    set_shortcut(penButtons[VECTOR2],   Qt::Key_W, Qt::AltModifier);
    set_shortcut(penButtons[CIRCLE],    Qt::Key_C, Qt::AltModifier);
    set_shortcut(penButtons[TRIANGLE],  Qt::Key_T, Qt::AltModifier);
    set_shortcut(penButtons[LINE],      Qt::Key_L, Qt::AltModifier);
    set_shortcut(penButtons[RECTANGLE], Qt::Key_R, Qt::AltModifier);
    set_shortcut(penButtons[SPLINE],    Qt::Key_S, Qt::AltModifier);

    int lineStyles[] = {NORMAL, DOTLINE, LINELINE, FILLED};
    for (int l : lineStyles) {
        penButtons[l] = create_button(l, [=](){
            setLineStyle(l);
        });
    }

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
        QColor newCol = QColorDialog::getColor(drawing->pen.color(), NULL, _("Select Color"));
        setHideMainWindow(false);
        floatingWidget->show();
        if(! newCol.isValid()){
            return;
        }
        drawing->pen.setColor(newCol);
        set_string("color", newCol.name());
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

    pen_init = true;
}
