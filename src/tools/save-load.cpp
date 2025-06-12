#include <QImage>
#include <QByteArray>
#include <QFile>

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

void openFile(QString filename){
    if(!filename.isEmpty()){
        drawing->clearAll();
        bgMenu->show();
        #ifdef QPRINTER
        if(filename.endsWith(".pdf")){
            loadPdf(filename);
            drawing->goPage(0);
            bgMenu->hide();
        } else {
        #endif
            archive_target = filename;
            load_archive(NULL);
        #ifdef QPRINTER
        }
        #endif
    }
}

void setupSaveLoad(){
#ifdef LIBARCHIVE
    toolButtons[SAVE] = create_button(SAVE, [=](){
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
        #ifdef QPRINTER
        } else if(selectedFilter.contains("pdf") && !file.endsWith(".pdf")){
            file += ".pdf";
        #endif
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

    toolButtons[OPEN] = create_button(OPEN, [=](){
        QString filter = _("Pen Files (*.pen);;");
        #ifdef QPRINTER
        filter += _("PDF Files (*.pdf);;");
        #endif
        filter += _("All Files (*.*)");
        setHideMainWindow(true);
        floatingWidget->hide();
        floatingSettings->setHide();
        QString filename = QFileDialog::getOpenFileName(drawing, _("Open File"), QDir::homePath(), filter);
        openFile(filename);
        floatingWidget->show();
        setHideMainWindow(false);
    });
    set_shortcut(toolButtons[OPEN], Qt::Key_O, Qt::ControlModifier);
#endif
}



bool saveImageToFile(const QImage &image, const QString &imageFilePath) {
    if (image.isNull()) {
        return false;
    }

    QFile imageFile(imageFilePath);
    if (!imageFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QByteArray imageData(reinterpret_cast<const char*>(image.constBits()), image.sizeInBytes());

    QDataStream out(&imageFile);
    out << image.width() << image.height() << static_cast<int>(image.format()) << imageData;
    imageFile.close();

    return true;
}
QImage loadImageFromFile(const QString &imageFilePath) {
    QFile imageFile(imageFilePath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        return QImage();
    }

    QDataStream in(&imageFile);
    int width, height, format;
    QByteArray loadedData;

    in >> width >> height >> format >> loadedData;

    imageFile.close();

    QImage loadedImage(reinterpret_cast<const uchar*>(loadedData.constData()), width, height, static_cast<QImage::Format>(format));
    return loadedImage.copy();
}


