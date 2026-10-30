#ifdef QT5
#include <poppler-qt5.h>
#else
#include <poppler-qt6.h>
#endif
#include <QImage>

#include <constants.h>
#include <widgets/DrawingWidget.h>
#include <widgets/WhiteBoard.h>


bool PDFMODE = false;

#ifdef QT5
static Poppler::Document *doc;
static Poppler::Page *page;
#else
static std::unique_ptr<Poppler::Document> doc;
static std::unique_ptr<Poppler::Page> page;
#endif

// Poppler returns null for a broken file and a locked document can't be read
// without its password. doc is replaced only by a good one, the board can still
// be using the pdf that was opened before.
template <typename T>
static bool set_pdf_mode(T new_doc){
    if(!new_doc || new_doc->isLocked()){
        printf("Failed to open pdf\n");
        return false;
    }
    doc = std::move(new_doc);
    PDFMODE = true;
    doc->setRenderHint(Poppler::Document::Antialiasing, true);
    doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
    drawing->max = doc->numPages()-1;
    drawing->goPage(0);
    updateGui();
    return true;
}

void loadPdfFromData(QByteArray data){
    set_pdf_mode(Poppler::Document::loadFromData(data));
}
bool loadPdf(QString path){
    return set_pdf_mode(Poppler::Document::load(path));
}

QImage getPdfImage(int num, float ratio){
    if(doc->numPages() <= num){
        return QImage(0,0);
    }
    page = doc->page(num);
    if(!page){
        return QImage(0,0);
    }
    float wrat =  (float) board->geometry().width() / (float) page->pageSize().width();
    float hrat =  (float) board->geometry().height() / (float) page->pageSize().height();
    float rat = MIN(wrat, hrat);
    return page->renderToImage(
        72*rat*ratio, 72*rat*ratio, 0, 0,
        page->pageSize().width()*rat*ratio,
        page->pageSize().height()*rat*ratio
    );
}
