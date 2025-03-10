#include "../tools.h"


void penStyleEvent(){
    backgroundStyleEvent();
    colorDialog->setVisible(drawing->penType != ERASER || drawing->penType == SELECTION);
    ov->setVisible(drawing->penType != SELECTION);
    thicknessSlider->setVisible(drawing->penType != SELECTION);
    thicknessLabel->setVisible(drawing->penType != SELECTION);
    modeDialog->setVisible(drawing->penType != ERASER && drawing->penType != SELECTION);
    penTypeDialog->setVisible(drawing->penType != ERASER && drawing->penType != SELECTION);
    penSwitch->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    if(drawing->penType == SELECTION){
        set_icon(":images/crop.svg", penSwitch);
    } else if (drawing->penType == ERASER){
        set_icon(":images/eraser.svg", penSwitch);
    } else if(drawing->penType == MARKER){
        set_icon(":images/marker.svg", penSwitch);
    } else {
        set_icon(":images/pen.svg", penSwitch);
    }

}


int last_pen_type = 0;
void penSizeEvent(){
    int value = drawing->penSize[drawing->penType];
    ov->updateImage();
    floatingSettings->reload();
    thicknessLabel->setText(QString(_("Size:"))+QString(" ")+QString::number(value));
}

void updateRatioButtons(){
    overlayScaleDown->setEnabled(board->ratios[drawing->getPageNum()] >= 30);
    overlayScaleUp->setEnabled(board->ratios[drawing->getPageNum()] <= 200);
}

void updateGoBackButtons(){
    backButton->setEnabled(drawing->isBackAvailable());
    nextButton->setEnabled(drawing->isNextAvailable());
    previousPage->setEnabled(drawing->getPageNum() > 0);
    pageLabel->setText(QString::number(drawing->getPageNum()));
}

void backgroundStyleEvent(){
    for(int i=0; i<1024; i++){
        if(penButtons[i] != nullptr){
            //printf("%d\n", i);
            penButtons[i]->setStyleSheet(QString("background-color: none;"));
        }
    }
    int btns[] = {
        drawing->penType, drawing->lineStyle,
        drawing->penStyle, board->getType(),
        board->getOverlayType()
    };
    for(int btn:btns){
        if(penButtons[btn] != nullptr){
            penButtons[btn]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
        }
    }
    ov->updateImage();
}


