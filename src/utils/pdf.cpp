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

QImage getPdfImage(int num){
    if(doc->numPages() <= num){
        return QImage(0,0);
    }
    page = doc->page(num);
    return page->renderToImage(72*scale, 72*scale, 0, 0, page->pageSize().width()*scale, page->pageSize().height()*scale);
}