#include "Storage.h"

#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QIcon>
#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QColor>
#include <QStandardPaths>

#include "../constants.h"
#include "../widgets/DrawingWidget.h"
#include "../widgets/WhiteBoard.h"
#include "../widgets/Button.h"

#ifdef QPRINTER
#include <QPrinter>
#include <QPageSize>
#endif
#ifdef LIBARCHIVE
#include "Archive.h"
#endif

extern int history;

// ----------------------------------------------------------------
// ValueStorage
// ----------------------------------------------------------------

int ValueStorage::size() {
    return values.size();
}

void ValueStorage::clear() {
    return values.clear();
}

void ValueStorage::saveValue(qint64 id, QPointF data) {
    values[id] = data;
}

QPointF ValueStorage::last() {
    return loadValue(values.size()-1);
}

QPointF ValueStorage::first() {
    return loadValue(0);
}

QPointF ValueStorage::loadValue(qint64 id) {
    if (values.contains(id)) {
        return values[id];
    } else {
        return QPointF(-1,-1);
    }
}

// ----------------------------------------------------------------
// GeometryStorage
// ----------------------------------------------------------------

void GeometryStorage::addValue(qint64 id, QPointF data) {
    values[id].saveValue(values[id].size(), data);
}

void GeometryStorage::saveValue(qint64 id, qint64 id2, QPointF data) {
    values[id].saveValue(id2, data);
}

QPointF GeometryStorage::last(qint64 id){
    return load(id).last();
}

QPointF GeometryStorage::first(qint64 id){
    return load(id).first();
}

int GeometryStorage::size(qint64 id) {
    return values[id].size();
}

ValueStorage GeometryStorage::load(qint64 id) {
    if (values.contains(id)) {
        return values[id];
    } else {
        ValueStorage v;
        return v;
    }
}

void GeometryStorage::clear(qint64 id) {
    return values[id].clear();
}

void GeometryStorage::clearAll() {
    for (auto it = values.begin(); it != values.end(); ++it) {
        values[it.key()].clear();
    }
    values.clear();
}

// ----------------------------------------------------------------
// CursorStorage
// ----------------------------------------------------------------

CursorStorage curs;

void CursorStorage::init(qint64 id){
    if(!images.contains(id)){
        labels[id] = new QLabel("");
        labels[id]->setStyleSheet(QString("background-color: none;"));
        images[id] = new QWidget(mainWidget);
        layouts[id] = new QVBoxLayout(images[id]);
        layouts[id]->addWidget(labels[id]);
        layouts[id]->setContentsMargins(0,0,0,0);
        layouts[id]->setSpacing(0);
        sizes[id] = 0;
    }
}

void CursorStorage::setPosition(qint64 id, QPointF data) {
    init(id);
    images[id]->move(QPoint(
        data.x() - sizes[id]/2,
        data.y() - sizes[id]/2
    ));
    images[id]->show();
    setCursor(id, sizes[id]);
}

void CursorStorage::setCursor(qint64 id, int size){
    init(id);
    if(sizes[id] == size){
        return;
    }
    sizes[id] = size;
    static QIcon cursorIcon = QIcon(get_icon_by_id(CURSOR));
    QPixmap pixmap = cursorIcon.pixmap(
        cursorIcon.actualSize(
            QSize(size, size)
        )
    );
    labels[id]->setFixedSize(size, size);
    images[id]->setFixedSize(size, size);
    labels[id]->setPixmap(pixmap);
}

void CursorStorage::clear(){
    for (auto it = images.begin(); it != images.end(); ++it) {
        delete it.value();
    }
    images.clear();
    labels.clear();
    layouts.clear();
    sizes.clear();
    drawing.clear();
    penType.clear();
}

void CursorStorage::hide(qint64 id){
    init(id);
    images[id]->hide();
}

// ----------------------------------------------------------------
// ImageStorage
// ----------------------------------------------------------------

ImageStorage images;

ImageStorage::ImageStorage()
    : cache_path(cache + generateRandomString(10) + "/")
{
}

void ImageStorage::saveValue(qint64 id, QImage data) {
    QDir dir;
    dir.mkpath(cache_path);
    saveImageToFile(data, cache_path + QString::number(id));
    if(id > history){
        remove(id - history);
        removed++;
    }
}

void ImageStorage::clear(){
    image_count = 0;
    last_image_num = 1;
    removed = 0;
    removeDirectory(cache_path);
}

QImage ImageStorage::loadValue(qint64 id) {
    if(removed >= id) {
        id =  removed +1;
    }
    QString filePath = cache_path + QString::number(id);
    QFile file(filePath);
    if(file.exists()){
        return loadImageFromFile(cache_path + QString::number(id));
    } else {
        QPixmap pix = QPixmap(mainWidget->size() * mainWidget->devicePixelRatio());
        pix.fill(QColor("transparent"));
        pix.setDevicePixelRatio(mainWidget->devicePixelRatio());
        return pix.toImage();
    }
}

void ImageStorage::remove(qint64 id){
    QString filePath = cache_path + QString::number(id);
    QFile file(filePath);
    if(file.exists()){
        file.remove();
    }
}

// ----------------------------------------------------------------
// PageStorage
// ----------------------------------------------------------------

PageStorage pages;

void PageStorage::saveValue(qint64 id, ImageStorage data) {
    values[id] = data;
}

void PageStorage::clear(){
    values.clear();
    last_page_num = 0;
    page_count = 0;
}

ImageStorage PageStorage::loadValue(qint64 id) {
    if (id > page_count){
        page_count = id;
    }
    if (values.contains(id)) {
        return values[id];
    } else {
        ImageStorage imgs = ImageStorage();
        imgs.pageType = board->getType();
        return imgs;
    }
}

#ifdef QPRINTER
void PageStorage::savePdf(const QString& filename){
    saveValue(last_page_num, images);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filename);
    printer.setFullPage(true);
    QSizeF imageSize(mainWidget->geometry().width(), mainWidget->geometry().height());
    QPageSize pageSize(imageSize, QPageSize::Point);
    printer.setPageSize(pageSize);
    printer.setResolution(72);
    QPainter painter(&printer);
    for(int i=0;i<=page_count;i++){
        QImage im = loadValue(i).loadValue(loadValue(i).last_image_num);
        im.setDevicePixelRatio(mainWidget->devicePixelRatio());
        painter.drawPixmap(0,0, board->backgrounds[i]);
        painter.drawImage(0,0, im);
        printer.newPage();
    }
    painter.end();
}
#endif

#ifdef LIBARCHIVE
void PageStorage::saveAll(const QString& filename){
    images.overlayType = board->getOverlayType();
    images.pageType = board->getType();
    values[last_page_num] = images;
    QString cfg = "[main]\n";
    cfg += "width="+QString::number(mainWidget->geometry().width())+"\n";
    cfg += "height="+QString::number(mainWidget->geometry().height())+"\n";
    for(int i=0;i<=page_count;i++){
        cfg += "[page"+QString::number(i)+"]\n";
        cfg += "overlay="+get_overlay_by_id(loadValue(i).overlayType)+"\n";
        cfg += "ratio="+QString::number(board->ratios[i])+"\n";
        cfg += "rotate="+QString::number(board->rotates[i])+"\n";
        cfg += "page="+get_overlay_by_id(loadValue(i).pageType)+"\n";
        archive_add(QString::number(i)+"/background", drawing->getOverlay(i));
        for(int j=1+loadValue(i).removed;j<=loadValue(i).image_count;j++){
            archive_add(QString::number(i)+"/"+QString::number(j-1-loadValue(i).removed), values[i].loadValue(j));
        }
    }
    archive_set_config(cfg);
#ifdef QPRINTER
    if(PDFMODE){
        archive_add_file("overlay.pdf", drawing->pdfPath);
    }
#endif
    archive_create(filename);
}

void PageStorage::loadArchive(const QString& filename){
    QMap<QString, QImage> archive = archive_load(filename);
    QString cfg = archive_get_config();
    clear();
    for (auto it = archive.begin(); it != archive.end(); ++it) {
        QString path = it.key();
        QStringList parts = path.split("/");
        QImage image = it.value();
        int page = parts[0].toInt();
        if(path.endsWith("background")){
            drawing->setOverlay(image, page);
            printf("Load: page bg: %d\n", page);
            continue;
        }
        int frame = parts[1].toInt();
        printf("Load: page: %d frame %d\n", page, frame);
        if(page > page_count){
            page_count = page;
        }
        if (!values.contains(page)) {
            ImageStorage data;
            values[page] = data;
            values[page].image_count = 0;
            values[page].last_image_num = 1;
        }
        image.setDevicePixelRatio(mainWidget->devicePixelRatio());
        values[page].saveValue(frame+1, image);
        values[page].image_count++;
        values[page].last_image_num = values[page].image_count;
    }
    QStringList list = cfg.split("\n");
    QString area = "main";
    int page = 0;
    for (const auto &str : list) {
        if(str.startsWith("[") && str.endsWith("]")) {
            area = str.mid(1,str.length()-2);
            if(area.startsWith("page")){
                page = area.mid(4,str.length()-1).toInt();
            }
        } else if(str.startsWith("overlay")){
            values[page].overlayType = get_id_by_overlay(str.split("=")[1]);
            debug("Load: page: %d overlay %d\n", page, values[page].overlayType);
        } else if(str.startsWith("page")){
            values[page].pageType = get_id_by_overlay(str.split("=")[1]);
            debug("Load: page: %d page %d\n", page, values[page].pageType);
        } else if(str.startsWith("ratio")){
            board->ratios[page] = str.split("=")[1].toInt();
            debug("Load: page: %d ratio %d\n", page, board->ratios[page]);
        } else if(str.startsWith("rotate")){
            board->rotates[page] = str.split("=")[1].toInt();
            debug("Load: page: %d rotates %d\n", page, board->rotates[page]);
        }
    }
    images = values[0];
    board->setType(images.pageType);
    board->setOverlayType(images.overlayType);
    drawing->loadImage(images.last_image_num);
    drawing->update();
    drawing->goPage(0);
    updateGui();
}
#endif
