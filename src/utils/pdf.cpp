#include <poppler-qt5.h>
#include <QImage>

#include "../tools.h"


bool PDFMODE = false;

static Poppler::Document *doc;
void loadPdf(QString path){
    PDFMODE = true;
    doc = Poppler::Document::load(path);
    doc->setRenderHint(Poppler::Document::Antialiasing, true);
    doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
}

static Poppler::Page *page;
QImage getPdfImage(int num){
    if(doc->numPages() <= num){
        return QImage(0,0);
    }
    page = doc->page(num);
    return page->renderToImage(72*2*scale, 72*2*scale, 0, 0, page->pageSize().width()*2*scale, page->pageSize().height()*2*scale);
}