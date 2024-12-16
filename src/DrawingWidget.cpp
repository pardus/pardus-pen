#include "DrawingWidget.h"
#include "WhiteBoard.h"

#ifdef QPRINTER
#include <QPrinter>
#endif
#ifdef LIBARCHIVE
#include "Archive.h"
#endif
#include <stdio.h>


#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#define _(String) gettext(String)

extern "C" {
#include "settings.h"
}

extern WhiteBoard *board;
extern QMainWindow* mainWindow;
extern DrawingWidget *drawing;

extern void updateGoBackButtons();
void removeDirectory(const QString &path);

#define padding 8

/*
penType:
 - 0 eraser
 - 1 pen
 - 2 marker
*/

#include <QDebug>
#include <QMap>

#ifdef QT5
#define points touchPoints
#define position pos
#endif

#ifndef HISTORY
#define HISTORY 15
#endif

class CursorStorage {
public:
    void init(qint64 id){
        if(!images.contains(id)){
            labels[id] = new QLabel("");
            labels[id]->setStyleSheet(QString("background-color: none;"));
            images[id] = new QWidget(mainWindow);
            images[id]->stackUnder(drawing);
            layouts[id] = new QVBoxLayout(images[id]);
            layouts[id]->addWidget(labels[id]);
            layouts[id]->setContentsMargins(0,0,0,0);
            layouts[id]->setSpacing(0);
            sizes[id] = 0;
        }
    }
    void setPosition(qint64 id, QPoint data) {
        init(id);
        //printf("%lld move\n", id);
        images[id]->move(QPoint(
            data.x() - sizes[id]/2,
            data.y() - sizes[id]/2
        ));
        images[id]->show();
        setCursor(id, sizes[id]);
    }

    void setCursor(qint64 id, int size){
        if(sizes[id] == size){
            return;
        }
        sizes[id] = size;
        QIcon icon = QIcon(":images/cursor.svg");
        QPixmap pixmap = icon.pixmap(
            icon.actualSize(
                QSize(size, size)
            )
        );
        //printf("%lld resize %d\n", id, size);
        labels[id]->setFixedSize(size, size);
        images[id]->setFixedSize(size, size);
        labels[id]->setPixmap(pixmap);
    }

    void hide(qint64 id){
        init(id);
        images[id]->hide();
    }

private:
    QMap<qint64, QWidget*> images;
    QMap<qint64, QLabel*> labels;
    QMap<qint64, QVBoxLayout*> layouts;
    QMap<qint64, int> sizes;
};
CursorStorage curs;

class ImageStorage {
public:
    int last_image_num = 1;
    int image_count = 0;
    int pageType = TRANSPARENT;
    int overlayType = NONE;
    int removed = 0;
    void saveValue(qint64 id, QImage data) {
        values[id] = data;
        updateGoBackButtons();
        if(id > HISTORY){
            remove(id-HISTORY);
            removed++;
        }
    }

    void clear(){
        values.clear();
        image_count = 0;
        last_image_num = 1;
        removed = 0;
        updateGoBackButtons();
    }

    QImage loadValue(qint64 id) {
        if(removed >= id) {
            id =  removed +1;
        }
        if (values.contains(id)) {
            return values[id];
        } else {
            QImage image = QImage(mainWindow->geometry().width(),mainWindow->geometry().height(), QImage::Format_ARGB32);
            image.fill(QColor("transparent"));
            return image;
        }
    }

    void remove(qint64 id){
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (it.key() == id) {
                values.erase(it);
                break;
            }
        }
    }

private:
    QMap<qint64, QImage> values;
};
ImageStorage images;


class PageStorage {
public:
    int last_page_num = 0;
    int page_count = 0;
    void saveValue(qint64 id, ImageStorage data) {
        values[id] = data;
    }

    void clear(){
        values.clear();
        last_page_num = 0;
        page_count = 0;
    }
#ifdef QPRINTER
    void savePdf(const QString& filename){
        saveValue(last_page_num, images);
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(filename);
        printer.setFullPage(true);
        QSizeF imageSize(mainWindow->geometry().width(),mainWindow->geometry().height());
        QPageSize pageSize(imageSize, QPageSize::Point);
        printer.setPageSize(pageSize);
        printer.setResolution(72); // TODO: replace this
            
        QPainter painter(&printer);
        for(int i=0;i<=page_count;i++){
            QImage im = loadValue(i).loadValue(loadValue(i).last_image_num);
            painter.drawImage(0,0, im);
            printer.newPage();
        }
        painter.end();
        
    }
#endif
#ifdef LIBARCHIVE
    void saveAll(const QString& filename){
        images.overlayType = board->getOverlayType();
        images.pageType = board->getType();
        values[last_page_num] = images;
        QString cfg = "[main]\n";
        cfg += "width="+QString::number(mainWindow->geometry().width())+"\n";
        cfg += "height="+QString::number(mainWindow->geometry().height())+"\n";
        for(int i=0;i<=page_count;i++){
            cfg += "[page"+QString::number(i)+"]\n";
            cfg += "overlay="+QString::number(loadValue(i).overlayType)+"\n";
            cfg += "page="+QString::number(loadValue(i).pageType)+"\n";
            for(int j=1+loadValue(i).removed;j<=loadValue(i).image_count;j++){
                archive_add(QString::number(i)+"/"+QString::number(j-1-loadValue(i).removed), values[i].loadValue(j));
            }
        }
        archive_set_config(cfg);
        archive_create(filename);
    }

    void loadArchive(const QString& filename){
        QMap<QString, QImage> archive = archive_load(filename);
        QString cfg = archive_get_config();
        clear();
        for (auto it = archive.begin(); it != archive.end(); ++it) {
            QString path = it.key();
            QStringList parts = path.split("/");
            QImage image = it.value();
            int page = parts[0].toInt();
            int frame = parts[1].toInt();
            printf("Load: page: %d frame %d\n", page, frame);
            if(page > page_count){
                page_count = page;
            }
            if (!values.contains(page)) {
                ImageStorage data;
                values[page] = data;
                values[page].image_count = 0;
                values[page].last_image_num = 0;
            }
            values[page].saveValue(frame+1, image);
            values[page].image_count++;
            values[page].last_image_num = values[page].image_count;
        }
        QStringList list = cfg.split("\n");
        QString area = "main";
        int page = 0;
        for (const auto &str : std::as_const(list)) {
            if(str.startsWith("[") && str.endsWith("]")) {
                area = str.mid(1,str.length()-2);
                if(area.startsWith("page")){
                    page = area.mid(4,str.length()-1).toInt();
                }
            } else if(str.startsWith("overlay")){
                values[page].overlayType = str.split("=")[1].toInt();
                printf("Load: page: %d overlay %d\n", page, values[page].overlayType);
            } else if(str.startsWith("page")){
                values[page].pageType = str.split("=")[1].toInt();
                printf("Load: page: %d page %d\n", page, values[page].pageType);
            }
        }
        images = values[0];
        board->setType(images.pageType);
        board->setOverlayType(images.overlayType);
        drawing->loadImage(images.last_image_num);
        drawing->update();
        updateGoBackButtons();
    }
#endif

    ImageStorage loadValue(qint64 id) {
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

private:
    QMap<qint64, ImageStorage> values;
};
PageStorage pages;

int curEventButtons = 0;

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    penType = PEN;
    penMode = DRAW;
    reset = true;
    setMouseTracking(true);
    cropWidget = new MovableWidget(mainWindow);
    cropWidget->stackUnder(this);
    QBoxLayout* cropLayout = new QVBoxLayout(cropWidget);
    cropLayout->addWidget(cropWidget->crop);
    cropLayout->setContentsMargins(0,0,0,0);
    cropLayout->setSpacing(0);
    cropWidget->setStyleSheet("border: 2px solid "+penColor.name()+";");
    cropWidget->hide();

    //QScreen *screen = QGuiApplication::primaryScreen();
    fpressure = get_int((char*)"pressure") / 100.0;
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::addPoint(int id, QPointF data) {
    if(geo.size(id) == 0) {
        geo.saveValue(id, 0, data);
        return;
    }
    if((lineStyle ==  NORMAL && penStyle == SPLINE) || penType == ERASER){
        geo.saveValue(id, 1, geo.load(id).loadValue(0));
        geo.saveValue(id, 0, data);
    } else {
        geo.addValue(id, data);
    }
}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
    drawing = true;
    mergeSelection();
    imageBackup = image;
    if(floatingSettings->isVisible()){
        floatingSettings->setHide();
    }
    geo.clear(-1);
    addPoint(-1,event->position());
    if(penMode != DRAW) {
        return;
    }
    int ev_pen = penType;
    if(event->buttons() & Qt::RightButton) {
        ev_pen = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        ev_pen = MARKER;
    }
    curs.init(-1);
    curs.setCursor(-1, penSize[ev_pen]);
    updateCursorMouse(-1, event->position());
    if(ev_pen != ERASER) {
        curs.hide(-1);
    }
    curEventButtons = event->buttons();

}

void DrawingWidget::updateCursorMouse(qint64 i, QPointF pos){
    updateCursorMouse(i, pos.toPoint());
}
void DrawingWidget::updateCursorMouse(qint64 i, QPoint pos){
    curs.setPosition(i, pos);
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    int penTypeBak = penType;
    if(event->buttons() & Qt::RightButton) {
        penType = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        penType = MARKER;
    }
    if (drawing) {
        switch(penMode) {
            case DRAW:
                updateCursorMouse(-1, event->position());
                addPoint(-1, event->position());
                drawLineToFunc(-1, 1.0);
                break;
            case SELECTION:
                selectionDraw(geo.first(-1), event->position());
                break;
        }
    }
    penType = penTypeBak;
}

void DrawingWidget::addImage(QImage img){
    images.last_image_num++;
    images.image_count = images.last_image_num;
    images.saveValue(images.last_image_num, img.copy());
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(curEventButtons & Qt::LeftButton && geo.size(-1) < 2) {
        addPoint(-1, event->position()+QPointF(0,1));
        drawLineToFunc(-1, 1.0);
    }
    if (drawing) {
       drawing = false;
    }
    if(penMode == SELECTION) {
        addPoint(-1, event->position());
        createSelection();
        update();
        return;
    }

    curEventButtons = 0;
    curs.hide(-1);
    addImage(image);
}

void DrawingWidget::initializeImage(const QSize &size) {
    image = QImage(size, QImage::Format_ARGB32);
    image.fill(QColor("transparent"));
}

void DrawingWidget::resizeEvent(QResizeEvent *event) {
    initializeImage(event->size());
    loadImage(images.last_image_num);
    QWidget::resizeEvent(event);
}

void DrawingWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(0, 0, image);
    painter.end();
}


void DrawingWidget::clear() {
    image.fill(QColor("transparent"));
    images.clear();
    update();
}



static QPointF last_end = QPointF(0,0);
static QPointF last_begin = QPointF(0,0);

void DrawingWidget::selectionDraw(QPointF startPoint, QPointF endPoint) {
    image = imageBackup;
    painter.begin(&image);
    painter.setPen(Qt::NoPen);
    penColor.setAlpha(127);
    painter.setBrush(QBrush(penColor));
    painter.drawRect(QRectF(startPoint,endPoint));
    painter.end();
    update(QRectF(last_begin, last_end).toRect().normalized().adjusted(-1, -1, +1, +1));
    update(QRectF(startPoint, endPoint).toRect().normalized().adjusted(-1, -1, +1, +1));

    last_begin = startPoint;
    last_end = endPoint;
}

#ifdef LIBARCHIVE
void DrawingWidget::saveAll(QString file){
    if (!file.isEmpty()) {
        if(file.endsWith(".pdf")){
            pages.savePdf(file);
            return;
        }
        if(!file.endsWith(".pen")){
            file += ".pen";
        }
        pages.saveAll(file);
    }
}

void DrawingWidget::loadArchive(const QString& filename){
    pages.loadArchive(filename);
}
#endif
void DrawingWidget::loadImage(int num){
    QImage img = images.loadValue(num);
    if(img.isNull()){
        return;
    }
    img = img.scaled(mainWindow->geometry().width(), mainWindow->geometry().height());
    QPainter p(&image);
    image.fill(QColor("transparent"));
    p.drawImage(QPointF(0,0), img);
    update();
}

void DrawingWidget::goNextPage(){
    images.overlayType = board->getOverlayType();
    images.pageType = board->getType();
    pages.saveValue(pages.last_page_num, images);
    pages.last_page_num++;
    images = pages.loadValue(pages.last_page_num);
    board->setType(images.pageType);
    board->setOverlayType(images.overlayType);
    loadImage(images.last_image_num);
}

void DrawingWidget::goPreviousPage(){
    images.overlayType = board->getOverlayType();
    images.pageType = board->getType();
    pages.saveValue(pages.last_page_num, images);
    pages.last_page_num--;
    images = pages.loadValue(pages.last_page_num);
    board->setType(images.pageType);
    board->setOverlayType(images.overlayType);
    loadImage(images.last_image_num);
}

void DrawingWidget::goPrevious(){
    if(!isBackAvailable()){
        return;
    }
    images.last_image_num--;
    loadImage(images.last_image_num);
}


void DrawingWidget::goNext(){
    if(!isNextAvailable()){
        return;
    }
    images.last_image_num++;
    loadImage(images.last_image_num);
}

bool tabletActive = false;

bool DrawingWidget::event(QEvent *ev) {
    switch (ev->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchEnd:
        case QEvent::TouchUpdate: {
            if(floatingSettings->isVisible()){
                floatingSettings->setHide();
            }
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->points();
            foreach(const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
                QPointF pos = touchPoint.position();
                if ((Qt::TouchPointState)touchPoint.state() == Qt::TouchPointPressed) {
                    geo.clear(touchPoint.id());
                    addPoint(touchPoint.id(), pos);
                    curs.init(touchPoint.id());
                    curs.setCursor(touchPoint.id(), penSize[penType]);
                }
                else if ((Qt::TouchPointState)touchPoint.state() == Qt::TouchPointReleased) {
                    geo.clear(touchPoint.id());
                    curs.hide(touchPoint.id());
                    update();
                    continue;
                }
                addPoint(touchPoint.id(), pos);
                drawLineToFunc(touchPoint.id(), touchPoint.pressure());
                updateCursorMouse(touchPoint.id(), pos.toPoint());
            }
            break;
        }
        case QEvent::TabletPress: {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(ev);
            geo.clear(-1);
            addPoint(-1, tabletEvent->position());
            imageBackup = image;
            tabletActive = true;
            break;
        }
        case QEvent::TabletRelease: {
            tabletActive = false;
            break;
        }
        case QEvent::TabletMove: {
            if(!tabletActive){
                break;
            }
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(ev);
            int penTypeBak = penType;
            if(tabletEvent->buttons() & Qt::RightButton) {
                penType = ERASER;
            }
            addPoint(-1, tabletEvent->position());
            drawLineToFunc(-1, tabletEvent->pressure());
            penType = penTypeBak;
        }

        default:
            break;
    }
    return QWidget::event(ev);
}

int DrawingWidget::getPageNum(){
    return pages.last_page_num;
}

bool DrawingWidget::isBackAvailable(){
    //printf("%d %d\n", images.last_image_num, images.image_count );
    return images.last_image_num > images.removed +1;
}

bool DrawingWidget::isNextAvailable(){
    //printf("%d %d\n", images.last_image_num, images.image_count );
    return images.last_image_num < images.image_count;
}


QColor convertColor(QColor color) {
    int tot =  color.red() + color.blue() + color.green();
    if (tot > 382) {
        return QColor(0,0,0, color.alpha());
    } else {
        return QColor(255, 255, 255, color.alpha());

    }
}

void removeDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists())
        return;

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden);
    foreach (const QFileInfo &fi, fileInfoList) {
        if (fi.isDir()) {
            removeDirectory(fi.absoluteFilePath());
        } else {
            QFile::remove(fi.absoluteFilePath());
        }
    }

    dir.rmdir(path);
}

void qImageToFile(const QImage& image, const QString& filename) {
    QFileInfo fileInfo(filename);
    QString dirname = fileInfo.dir().absolutePath();
    QDir dir(dirname);
    if (!dir.exists(dirname)) {
        dir.mkpath(dirname);
    }
    QPixmap pixmap = QPixmap::fromImage(image);
      if (pixmap.save(filename, "PNG")) {
            qDebug() << "Image saved successfully as" << filename;
        } else {
            qDebug() << "Failed to save image at" << filename;
        }
}

