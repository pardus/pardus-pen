#include "../tools.h"

QPushButton* pageLabel;

void setupBackground(){
    penButtons[TRANSPARENT] = create_button(TRANSPARENT, [=](){
        board->setType(TRANSPARENT);
    });
    set_shortcut(penButtons[TRANSPARENT], Qt::Key_Q, Qt::AltModifier);


    penButtons[BLACK] = create_button(BLACK, [=](){
        board->setType(BLACK);
    });
    set_shortcut(penButtons[BLACK], Qt::Key_W, Qt::AltModifier);

    penButtons[WHITE] = create_button(WHITE, [=](){
        board->setType(WHITE);
    });
    set_shortcut(penButtons[WHITE], Qt::Key_E, Qt::AltModifier);

    // page buttons
    toolButtons[PREVPAGE] = create_button(PREVPAGE, [=](){
        if(drawing->getPageNum() == 0){
            return;
        }
        drawing->goPreviousPage();
    });
    set_shortcut(toolButtons[PREVPAGE], Qt::Key_PageDown, Qt::ControlModifier);

    toolButtons[NEXTPAGE] = create_button(NEXTPAGE, [=](){
        drawing->goNextPage();
    });
    set_shortcut(toolButtons[NEXTPAGE], Qt::Key_PageUp, Qt::ControlModifier);

    // overlay buttons
    penButtons[BLANK] = create_button(BLANK, [=](){
        board->setOverlayType(BLANK);
    });

    penButtons[SQUARES] = create_button(SQUARES, [=](){
        board->setOverlayType(SQUARES);

    });

    penButtons[LINES] = create_button(LINES, [=](){
        board->setOverlayType(LINES);

    });

    penButtons[ISOMETRIC] = create_button(ISOMETRIC, [=](){
        board->setOverlayType(ISOMETRIC);
    });

    penButtons[MUSIC] = create_button(MUSIC, [=](){
        board->setOverlayType(MUSIC);
    });

    penButtons[CUSTOM] = create_button(CUSTOM, [=](){
        QString filter = QString(_("Images")) + QString("(*.png *.xpm *.jpg *.jpeg *.bmp *.gif *.svg);;");
        filter += _("All Files (*.*)");
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        QString filename = QFileDialog::getOpenFileName(drawing, _("Open Image File"), QDir::homePath(), filter);
        if (!filename.isEmpty()) {
            drawing->setOverlay(QImage(filename), drawing->getPageNum());
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

    toolButtons[OVERLAYSCALEUP] = create_button(OVERLAYSCALEUP, [=](){
        board->ratios[drawing->getPageNum()] += 10;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYSCALEDOWN] = create_button(OVERLAYSCALEDOWN, [=](){
        board->ratios[drawing->getPageNum()] -= 10;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYROTATEUP] = create_button(OVERLAYROTATEUP, [=](){
        board->rotates[drawing->getPageNum()] += 15;
        board->updateTransform();
        board->update();
    });

    toolButtons[OVERLAYROTATEDOWN] = create_button(OVERLAYROTATEDOWN, [=](){
        board->rotates[drawing->getPageNum()] -= 15;
        board->updateTransform();
        board->update();
    });

    toolButtons[CLEAR] = create_button(CLEAR, [=](){
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
