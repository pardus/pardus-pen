#include "../tools.h"

QLabel* pageLabel;

QPushButton* overlayScaleUp;
QPushButton* overlayScaleDown;

QPushButton* overlayRotateUp;
QPushButton* overlayRotateDown;

QPushButton *clear;

void setupBackground(){
    penButtons[TRANSPARENT] = create_button(":images/paper-transparent.svg", [=](){
        board->setType(TRANSPARENT);
        backgroundStyleEvent();
    });
    set_shortcut(penButtons[TRANSPARENT], Qt::Key_Q, Qt::AltModifier);


    penButtons[BLACK] = create_button(":images/paper-black.svg", [=](){
        board->setType(BLACK);
        backgroundStyleEvent();
    });
    set_shortcut(penButtons[BLACK], Qt::Key_W, Qt::AltModifier);

    penButtons[WHITE] = create_button(":images/paper-white.svg", [=](){
        board->setType(WHITE);
        backgroundStyleEvent();
    });
    set_shortcut(penButtons[WHITE], Qt::Key_E, Qt::AltModifier);

    // page buttons
    previousPage = create_button(":images/go-page-previous.svg", [=](){
        if(drawing->getPageNum() == 0){
            return;
        }
        drawing->goPreviousPage();
        backgroundStyleEvent();
        updateGoBackButtons();
    });
    set_shortcut(previousPage, Qt::Key_PageDown, Qt::ControlModifier);

    nextPage = create_button(":images/go-page-next.svg", [=](){
        drawing->goNextPage();
        backgroundStyleEvent();
        updateGoBackButtons();
    });
    set_shortcut(nextPage, Qt::Key_PageUp, Qt::ControlModifier);

    // overlay buttons
    penButtons[BLANK] = create_button(":images/overlay-none.svg", [=](){
        board->setOverlayType(BLANK);
        backgroundStyleEvent();
    });

    penButtons[SQUARES] = create_button(":images/overlay-squares.svg", [=](){
        board->setOverlayType(SQUARES);
        backgroundStyleEvent();
    });

    penButtons[LINES] = create_button(":images/overlay-lines.svg", [=](){
        board->setOverlayType(LINES);
        backgroundStyleEvent();
    });

    penButtons[ISOMETRIC] = create_button(":images/overlay-isometric.svg", [=](){
        board->setOverlayType(ISOMETRIC);
        backgroundStyleEvent();
    });

    penButtons[MUSIC] = create_button(":images/overlay-music.svg", [=](){
        board->setOverlayType(MUSIC);
        backgroundStyleEvent();
    });

    penButtons[CUSTOM] = create_button(":images/overlay-custom.svg", [=](){
        QString filter = QString(_("Images")) + QString("(*.png *.xpm *.jpg *.jpeg *.bmp *.gif *.svg)");
        filter += _("All Files (*.*)");
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        QString filename = QFileDialog::getOpenFileName(drawing, _("Open Image File"), QDir::homePath(), filter);
        if (!filename.isEmpty()) {
            board->overlays[drawing->getPageNum()] = QImage(filename);
            board->ratios[drawing->getPageNum()] = 100;
            updateRatioButtons();
        }
        floatingWidget->show();
        setHideMainWindow(false);
        board->setOverlayType(CUSTOM);
        backgroundStyleEvent();
    });
    set_shortcut(penButtons[CUSTOM], Qt::Key_O, Qt::AltModifier);

    pageLabel = new QLabel("0");

    overlayScaleUp = create_button(":images/zoom-in.svg", [=](){
        board->ratios[drawing->getPageNum()] += 10;
        updateRatioButtons();
        board->update();
    });

    overlayScaleDown = create_button(":images/zoom-out.svg", [=](){
        board->ratios[drawing->getPageNum()] -= 10;
        updateRatioButtons();
        board->update();
    });

    overlayRotateUp = create_button(":images/bg-rotate-plus.svg", [=](){
        board->rotates[drawing->getPageNum()] += 15;
        board->update();
    });

    overlayRotateDown = create_button(":images/bg-rotate-minus.svg", [=](){
        board->rotates[drawing->getPageNum()] -= 15;
        board->update();
    });

    clear = create_button(":images/clear.svg", [=](){
        if(drawing->penType == SELECTION && hasSelection) {
            drawing->clearSelection();
        } else {
            drawing->clear();
        }
    });
    set_shortcut(clear, Qt::Key_Delete, Qt::ControlModifier);

}
