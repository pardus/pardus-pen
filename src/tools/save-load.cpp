#include "../tools.h"


#ifdef LIBARCHIVE
extern "C" {
    QString archive_target;
    void *save_all(void* arg) {
        (void)arg;
        drawing->saveAll(archive_target);
        return NULL;
    }
    void *load_archive(void* arg) {
        (void)arg;
        drawing->loadArchive(archive_target);
        return NULL;
    }
}
#endif

void setupSaveLoad(){
#ifdef LIBARCHIVE
    toolButtons[SAVE] = create_button(":images/save.svg", [=](){
        QString filter = _("Pen Files (*.pen);;");
        #ifdef QPRINTER
        filter += _("PDF Files (*.pdf);;");
        #endif
        filter += _("All Files (*.*)");
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        QString selectedFilter;
        QString file = QFileDialog::getSaveFileName(drawing, _("Save File"), QDir::homePath(), filter, &selectedFilter);
        if(selectedFilter.contains("pen") && !file.endsWith(".pen")){
            file += ".pen";
        } else if(selectedFilter.contains("pdf") && !file.endsWith(".pdf")){
            file += ".pdf";
        }
        // save current page using goPage()
        drawing->goPage(drawing->getPageNum());
        pthread_t ptid;
        // Creating a new thread
        archive_target = file;
        pthread_create(&ptid, NULL, &save_all, NULL);
        floatingWidget->show();
        setHideMainWindow(false);
    });
    set_shortcut(toolButtons[SAVE], Qt::Key_S, Qt::ControlModifier);

    toolButtons[OPEN] = create_button(":images/open.svg", [=](){
        QString filter = _("Pen Files (*.pen);;");
        filter += _("All Files (*.*)");
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        QString filename = QFileDialog::getOpenFileName(drawing, _("Open File"), QDir::homePath(), filter);
        if(!filename.isEmpty()){
            pthread_t ptid;
            archive_target = filename;
            pthread_create(&ptid, NULL, &load_archive, NULL);
        }
        floatingWidget->show();
        setHideMainWindow(false);
    });
    set_shortcut(toolButtons[OPEN], Qt::Key_O, Qt::ControlModifier);
#endif
}
