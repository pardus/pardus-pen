#include "../tools.h"


void penStyleEvent(){
    backgroundStyleEvent();
    colorDialog->setVisible(getPen() != ERASER || getPen() == SELECTION);
    ov->setVisible(getPen() != SELECTION);
    thicknessSlider->setVisible(getPen() != SELECTION);
    thicknessLabel->setVisible(getPen() != SELECTION);
    modeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);
    penTypeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);
    toolButtons[SWITCH]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    if(getPen() == SELECTION){
        set_icon(":images/crop.svg", toolButtons[SWITCH]);
    } else if (getPen() == ERASER){
        set_icon(":images/eraser.svg", toolButtons[SWITCH]);
    } else if(getPen() == MARKER){
        set_icon(":images/marker.svg", toolButtons[SWITCH]);
    } else {
        set_icon(":images/pen.svg", toolButtons[SWITCH]);
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
    toolButtons[OVERLAYSCALEUP]->setEnabled(board->ratios[drawing->getPageNum()] >= 30);
    toolButtons[OVERLAYSCALEDOWN]->setEnabled(board->ratios[drawing->getPageNum()] <= 200);
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


