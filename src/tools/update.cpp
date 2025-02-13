#include "../tools.h"


void penStyleEvent(){
    penButton->setStyleSheet(QString("background-color: none;"));
    selectButton->setStyleSheet(QString("background-color: none;"));
    markerButton->setStyleSheet(QString("background-color: none;"));
    eraserButton->setStyleSheet(QString("background-color: none;"));
    lineButton->setStyleSheet(QString("background-color: none;"));
    splineButton->setStyleSheet(QString("background-color: none;"));
    circleButton->setStyleSheet(QString("background-color: none;"));
    rectButton->setStyleSheet(QString("background-color: none;"));
    triangleButton->setStyleSheet(QString("background-color: none;"));

    lineDotLineButton->setStyleSheet(QString("background-color: none;"));
    lineNormalButton->setStyleSheet(QString("background-color: none;"));
    lineLineLineButton->setStyleSheet(QString("background-color: none;"));
    switch(drawing->lineStyle){
        case NORMAL:
            lineNormalButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case LINELINE:
            lineLineLineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case DOTLINE:
            lineDotLineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
    }
    switch(drawing->penStyle){
        case LINE:
            lineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case CIRCLE:
            circleButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case RECTANGLE:
            rectButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case TRIANGLE:
            triangleButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        default:
            splineButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
    }
    switch(drawing->penType){
        case SELECTION:
            selectButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case PEN:
            penButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case MARKER:
            markerButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        default:
            eraserButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
    }
    ov->penSize = drawing->penSize[drawing->penType];
    ov->penType = drawing->penType;
    ov->color = drawing->penColor;
    ov->updateImage();
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

    ov->penSize = value;
    ov->color = drawing->penColor;
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
    transparentButton->setStyleSheet(QString("background-color: none;"));
    blackButton->setStyleSheet(QString("background-color: none;"));
    whiteButton->setStyleSheet(QString("background-color: none;"));

    overlayIsometric->setStyleSheet(QString("background-color: none;"));
    overlayMusic->setStyleSheet(QString("background-color: none;"));
    overlayLines->setStyleSheet(QString("background-color: none;"));
    overlaySquares->setStyleSheet(QString("background-color: none;"));
    overlayNone->setStyleSheet(QString("background-color: none;"));
    overlayCustom->setStyleSheet(QString("background-color: none;"));
    drawing->cropWidget->setStyleSheet("border: 2px solid "+drawing->penColor.name()+";");
    switch(board->getType()){
        case BLACK:
            blackButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            ov->background = Qt::black;
            break;
        case WHITE:
            whiteButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            ov->background = Qt::white;
            break;
        default:
            transparentButton->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            ov->background = Qt::transparent;
            break;
    }
    switch(board->getOverlayType()){
        
        case LINES:
            overlayLines->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case ISOMETRIC:
            overlayIsometric->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case MUSIC:
            overlayMusic->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        case SQUARES:
            overlaySquares->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
       case CUSTOM:
            overlayCustom->setStyleSheet("background-color:"+drawing->penColor.name()+";");
            break;
        default:
            overlayNone->setStyleSheet("background-color:"+drawing->penColor.name()+";");
    }
    ov->updateImage();
}


