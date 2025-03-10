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
    switch(drawing->penType){
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
    penButtons[drawing->penType]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    penButtons[drawing->lineStyle]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    penButtons[drawing->penStyle]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    penButtons[board->getType()]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    penButtons[board->getOverlayType()]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    ov->updateImage();
}


