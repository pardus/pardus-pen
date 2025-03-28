#include "../tools.h"


void penStyleEvent(){
    backgroundStyleEvent();
    colorDialog->setVisible(getPen() != ERASER || getPen() == SELECTION);
    ov->setVisible(getPen() != SELECTION);
    thicknessSlider->setVisible(getPen() != SELECTION);
    thicknessLabel->setVisible(getPen() != SELECTION);
    modeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);
    penTypeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);
    toolButtons[PENMENU]->setStyleSheet("background-color: none;");
    toolButtons[ERASERMENU]->setStyleSheet("background-color: none;");
    if(drawing->getPen() == PEN){
        toolButtons[PENMENU]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    } else if (drawing->getPen() == ERASER){
        toolButtons[ERASERMENU]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    }
    switch(drawing->getPenStyle()){
        case LINE:
            set_icon(":images/line.svg", toolButtons[SHAPEMENU]);
            break;
        case CIRCLE:
            set_icon(":images/circle.svg", toolButtons[SHAPEMENU]);
            break;
        case RECTANGLE:
            set_icon(":images/rectangle.svg", toolButtons[SHAPEMENU]);
            break;
        case TRIANGLE:
            set_icon(":images/triangle.svg", toolButtons[SHAPEMENU]);
            break;
        case VECTOR:
            set_icon(":images/vector.svg", toolButtons[SHAPEMENU]);
            break;
        case VECTOR2:
            set_icon(":images/vector2.svg", toolButtons[SHAPEMENU]);
            break;
        default:
            set_icon(":images/spline.svg", toolButtons[SHAPEMENU]);
            break;
    }

}


int last_pen_type = 0;
void penSizeEvent(){
    int value = drawing->penSize[getPen()];
    ov->updateImage();
    floatingSettings->reload();
    thicknessLabel->setText(QString(_("Size:"))+QString(" ")+QString::number(value));
}

void updateRatioButtons(){
    toolButtons[OVERLAYSCALEDOWN]->setEnabled(board->ratios[drawing->getPageNum()] >= 30);
    toolButtons[OVERLAYSCALEUP]->setEnabled(board->ratios[drawing->getPageNum()] <= 200);
}

void updateGoBackButtons(){
    toolButtons[BACK]->setEnabled(drawing->isBackAvailable());
    toolButtons[NEXT]->setEnabled(drawing->isNextAvailable());
    toolButtons[PREVPAGE]->setEnabled(drawing->getPageNum() > 0);
    pageLabel->setText(QString::number(drawing->getPageNum()));
}

void backgroundStyleEvent(){
    for (auto it = penButtons.begin(); it != penButtons.end(); ++it) {
        it.value()->setStyleSheet(QString("background-color: none;"));
    }
    int btns[] = {
        getPen(), drawing->getLineStyle(),
        drawing->getPenStyle(), board->getType(),
        board->getOverlayType()
    };
    for(int btn:btns){
        if(penButtons[btn] != nullptr){
            penButtons[btn]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
        }
    }
    ov->updateImage();
}


