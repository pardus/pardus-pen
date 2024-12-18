#include "tools.h"

QPushButton* transparentButton;
QPushButton* blackButton;
QPushButton* whiteButton;

QPushButton* overlayIsometric;
QPushButton* overlayMusic;
QPushButton* overlayCustom;
QPushButton* overlayTurkiye;
QPushButton* overlayWorld;
QPushButton* overlayLines;
QPushButton* overlaySquares;
QPushButton* overlayNone;

QPushButton *clear;

void setupBackground(){
    transparentButton = create_button(":images/paper-transparent.svg", [=](){
        board->setType(TRANSPARENT);
        backgroundStyleEvent();
    });
    blackButton = create_button(":images/paper-black.svg", [=](){
        board->setType(BLACK);
        backgroundStyleEvent();
    });
    whiteButton = create_button(":images/paper-white.svg", [=](){
        board->setType(WHITE);
        backgroundStyleEvent();
    });
    
    // page buttons
    previousPage = create_button(":images/go-page-previous.svg", [=](){
        if(drawing->getPageNum() == 0){
            return;
        }
        drawing->goPreviousPage();
        backgroundStyleEvent();
        updateGoBackButtons();
    });

    nextPage = create_button(":images/go-page-next.svg", [=](){
        drawing->goNextPage();
        backgroundStyleEvent();
        updateGoBackButtons();
    });

    // overlay buttons
    overlayNone = create_button(":images/overlay-none.svg", [=](){
        board->setOverlayType(NONE);
        backgroundStyleEvent();
    });

    overlaySquares = create_button(":images/overlay-squares.svg", [=](){
        board->setOverlayType(SQUARES);
        backgroundStyleEvent();
    });

    overlayLines = create_button(":images/overlay-lines.svg", [=](){
        board->setOverlayType(LINES);
        backgroundStyleEvent();
    });

    overlayIsometric = create_button(":images/overlay-isometric.svg", [=](){
        board->setOverlayType(ISOMETRIC);
        backgroundStyleEvent();
    });

    overlayMusic = create_button(":images/overlay-music.svg", [=](){
        board->setOverlayType(MUSIC);
        backgroundStyleEvent();
    });
    
    overlayCustom = create_button(":images/overlay-custom.svg", [=](){
        board->setOverlayType(CUSTOM);
        backgroundStyleEvent();
    });

    overlayTurkiye = create_button(":images/turkiye-map.svg", [=](){
        board->setOverlayType(TURKIYE);
        backgroundStyleEvent();
    });
    overlayWorld = create_button(":images/world-map.svg", [=](){
        board->setOverlayType(WORLD);
        backgroundStyleEvent();
    });
    
    
    
    clear = create_button(":images/clear.svg", [=](){
        if(drawing->penMode == SELECTION && hasSelection) {
            drawing->clearSelection();
        } else {
            drawing->clear();
        }
    });
}
