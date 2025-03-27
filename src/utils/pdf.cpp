#ifdef QT5
#include <poppler-qt5.h>
#else
#include <poppler-qt6.h>
#endif
#include <QImage>

#include "../tools.h"


bool PDFMODE = false;

#ifdef QT5
static Poppler::Document *doc;
static Poppler::Page *page;
#else
static std::unique_ptr<Poppler::Document> doc;
static std::unique_ptr<Poppler::Page> page;
#endif

void loadPdf(QString path){
    PDFMODE = true;
    doc = Poppler::Document::load(path);
    doc->setRenderHint(Poppler::Document::Antialiasing, true);
    doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
}

QImage getPdfImage(int num, float ratio){
    if(doc->numPages() <= num){
        return QImage(0,0);
    }
    page = doc->page(num);
    float wrat =  (float) board->geometry().width() / (float) page->pageSize().width();
    float hrat =  (float) board->geometry().height() / (float) page->pageSize().height();
    float rat = MIN(wrat, hrat);
    return page->renderToImage(
        72*rat*ratio, 72*rat*ratio, 0, 0,
        page->pageSize().width()*rat*ratio,
        page->pageSize().height()*rat*ratio
    );
}
