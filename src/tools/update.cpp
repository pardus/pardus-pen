#include "../tools.h"

int last_pen_type = 0;

void updateGui(){
    // hide or show elements
    colorDialog->setVisible(getPen() != ERASER || getPen() == SELECTION);
    ov->setVisible(getPen() != SELECTION);
    thicknessSlider->setVisible(getPen() != SELECTION);
    thicknessLabel->setVisible(getPen() != SELECTION);
    modeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);
    penTypeDialog->setVisible(getPen() != ERASER && getPen() != SELECTION);

    // pen and eraser menu
    toolButtons[PENMENU]->setStyleSheet("background-color: none;");
    toolButtons[ERASERMENU]->setStyleSheet("background-color: none;");

    if(drawing->getPen() == MARKER || drawing->getPen() == PEN){
        set_icon_combined(get_icon_by_id(drawing->getPen()), get_icon_by_id(drawing->getPenStyle()), toolButtons[PENMENU]);
        toolButtons[PENMENU]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    } else if (drawing->getPen() == ERASER){
        toolButtons[ERASERMENU]->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    }
    // Update button backgrounds
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
    // Update pen size
    int value = drawing->penSize[getPen()];
    thicknessLabel->setText(QString(_("Size:"))+QString(" ")+QString::number(value));


    // update go back buttons
    toolButtons[BACK]->setEnabled(drawing->isBackAvailable());
    toolButtons[NEXT]->setEnabled(drawing->isNextAvailable());
    toolButtons[PREVPAGE]->setEnabled(drawing->getPageNum() > 0);
    toolButtons[NEXTPAGE]->setEnabled(drawing->getPageNum() < drawing->max);
    pageLabel->setText(QString::number(drawing->getPageNum()));

    // update scale buttons
    toolButtons[OVERLAYSCALEDOWN]->setEnabled(board->ratios[drawing->getPageNum()] >= 30);
    toolButtons[OVERLAYSCALEUP]->setEnabled(board->ratios[drawing->getPageNum()] <= 200);
    // update overview
    ov->updateImage();
    floatingSettings->reload();
}



