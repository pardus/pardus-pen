#include "../tools.h"

QPushButton* pageLabel;

void setupBackground(){
    penButtons[TRANSPARENT] = create_button(":images/paper-transparent.svg", [=](){
        board->setType(TRANSPARENT);
    });
    set_shortcut(penButtons[TRANSPARENT], Qt::Key_Q, Qt::AltModifier);


    penButtons[BLACK] = create_button(":images/paper-black.svg", [=](){
        board->setType(BLACK);
    });
    set_shortcut(penButtons[BLACK], Qt::Key_W, Qt::AltModifier);

    penButtons[WHITE] = create_button(":images/paper-white.svg", [=](){
        board->setType(WHITE);
    });
    set_shortcut(penButtons[WHITE], Qt::Key_E, Qt::AltModifier);

    // page buttons
    toolButtons[PREVPAGE] = create_button(":images/go-page-previous.svg", [=](){
        if(drawing->getPageNum() == 0){
            return;
        }
        drawing->goPreviousPage();
    });
    set_shortcut(toolButtons[PREVPAGE], Qt::Key_PageDown, Qt::ControlModifier);

    toolButtons[NEXTPAGE] = create_button(":images/go-page-next.svg", [=](){
        drawing->goNextPage();
    });
    set_shortcut(toolButtons[NEXTPAGE], Qt::Key_PageUp, Qt::ControlModifier);

    // overlay buttons
    penButtons[BLANK] = create_button(":images/overlay-none.svg", [=](){
        board->setOverlayType(BLANK);
    });

    penButtons[SQUARES] = create_button(":images/overlay-squares.svg", [=](){
        board->setOverlayType(SQUARES);

    });

    penButtons[LINES] = create_button(":images/overlay-lines.svg", [=](){
        board->setOverlayType(LINES);

    });

    penButtons[ISOMETRIC] = create_button(":images/overlay-isometric.svg", [=](){
        board->setOverlayType(ISOMETRIC);
    });

    penButtons[MUSIC] = create_button(":images/overlay-music.svg", [=](){
        board->setOverlayType(MUSIC);
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
            board->rotates[drawing->getPageNum()] = 0;
            board->updateTransform();
            board->setOverlayType(CUSTOM);
        } else {
            board->setOverlayType(BLANK);
        }
        floatingWidget->show();
        setHideMainWindow(false);
    });
    set_shortcut(penButtons[CUSTOM], Qt::Key_O, Qt::AltModifier);

    pageLabel = create_button_text("0", [=](){
        bool ok = false;
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        int number = QInputDialog::getInt(mainWidget, _("Go page"),
                                           _("Page:"), 0, 0, drawing->max, 1, &ok);
        if (ok) {
            drawing->goPage(number);
        }
        setHideMainWindow(false);
        floatingWidget->show();
    });

    toolButtons[OVERLAYSCALEUP] = create_button(":images/zoom-in.svg", [=](){
        board->ratios[drawing->getPageNum()] += 10;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYSCALEDOWN] = create_button(":images/zoom-out.svg", [=](){
        board->ratios[drawing->getPageNum()] -= 10;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYROTATEUP] = create_button(":images/bg-rotate-plus.svg", [=](){
        board->rotates[drawing->getPageNum()] += 15;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYROTATEDOWN] = create_button(":images/bg-rotate-minus.svg", [=](){
        board->rotates[drawing->getPageNum()] -= 15;
        board->updateTransform();
        board->update();
    });

    toolButtons[CLEAR] = create_button(":images/clear.svg", [=](){
        if(getPen() == SELECTION && hasSelection) {
            drawing->clearSelection();
        } else {
            drawing->clear();
        }
        floatingSettings->setHide();
        setPen(PEN);
        setPenStyle(SPLINE);
    });
    set_shortcut(toolButtons[CLEAR], Qt::Key_Delete, Qt::ControlModifier);

}
