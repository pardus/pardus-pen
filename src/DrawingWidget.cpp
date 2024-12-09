#include "DrawingWidget.h"
#include "WhiteBoard.h"
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

class ValueStorage {
public:
    void saveValue(qint64 id, QPointF data) {
        values[id] = data;
    }

    QPointF loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            // -1 -1 means disable drawing
            return QPointF(-1,-1);
        }
    }

private:
    QMap<qint64, QPointF> values;
};
ValueStorage storage;

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
bool isMoved = 0;
float fpressure = 0;

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    penType = 1;
    reset = true;
    setMouseTracking(true);
    //QScreen *screen = QGuiApplication::primaryScreen();
    fpressure = get_int((char*)"pressure") / 100.0;
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
    updateCursorMouse(-1, event->position());
    int ev_pen = penType;
    if(event->buttons() & Qt::RightButton) {
        ev_pen = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        ev_pen = MARKER;
    }
    curs.setCursor(-1, penSize[ev_pen]);
    drawing = true;
    lastPoint = event->position();
    firstPoint = event->position();
    imageBackup = image;
    curEventButtons = event->buttons();
    isMoved = false;
    if(floatingSettings->isVisible()){
        floatingSettings->hide();
    }
}

void DrawingWidget::updateCursorMouse(qint64 i, QPointF pos){
    updateCursorMouse(i, pos.toPoint());
}
void DrawingWidget::updateCursorMouse(qint64 i, QPoint pos){
    if(penType != ERASER){
        curs.hide(i);
    } else {
        curs.setPosition(i, pos);
    }
}

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    int penTypeBak = penType;
    if(event->buttons() & Qt::RightButton) {
        penType = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        penType = MARKER;
    }
    if (drawing) {
        updateCursorMouse(-1, event->position());
        drawLineTo(event->position());
    } else {
        curs.hide(-1);
    }
    isMoved = true;
    penType = penTypeBak;
}

void DrawingWidget::mouseReleaseEvent(QMouseEvent *event) {
    if(curEventButtons & Qt::LeftButton && !isMoved) {
        drawLineTo(event->position()+QPointF(0,1));
    }
    if (drawing) {
       drawing = false;
    }
    images.last_image_num++;
    images.image_count = images.last_image_num;
    images.saveValue(images.last_image_num, image.copy());
    curEventButtons = 0;
    curs.hide(-1);
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


int rad = 0;
int frad = 0;

void DrawingWidget::drawLineTo(const QPointF &endPoint) {
    drawLineToFunc(lastPoint, endPoint, 1.0);
    lastPoint = endPoint;
}
static QPointF last_end = QPointF(0,0);
static QPointF last_begin = QPointF(0,0);

void DrawingWidget::drawLineToFunc(QPointF startPoint, QPointF endPoint, qreal pressure) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
    }
    if (fpressure > 0){
        pressure = fpressure;
    }
    int fpenStyle =  penStyle;
    if (penType == ERASER) {
        fpenStyle = SPLINE;
    }

    switch(fpenStyle){
        case SPLINE:
            painter.begin(&image);
            break;
        case LINE:
        case CIRCLE:
        case RECTANGLE:
        case TRIANGLE:
            startPoint = firstPoint;
            image = imageBackup;
            painter.begin(&image);
            break;
    }
    penColor.setAlpha(255);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    switch(penType){
        case PEN:
            break;
        case ERASER:
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            break;
        case MARKER:
            penColor.setAlpha(127);
            break;
    }

    painter.setPen(QPen(penColor, penSize[penType]*pressure, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    switch(fpenStyle){
        case SPLINE:
            painter.drawLine(startPoint, endPoint);
            break;
        case LINE:
            painter.drawLine(startPoint, endPoint);
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length();
            painter.drawEllipse(startPoint, rad, rad);
            break;
        case RECTANGLE:
            painter.drawRect(QRectF(startPoint,endPoint));
            break;
        case TRIANGLE:
            painter.drawLine(startPoint, endPoint);
            painter.drawLine(startPoint, QPointF(startPoint.x(), endPoint.y()));
            painter.drawLine(QPointF(startPoint.x(), endPoint.y()), endPoint);
            break;
    }
    switch(fpenStyle){
        case SPLINE:
            rad = penSize[penType];
            update(QRectF(
                startPoint, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case LINE:
        case TRIANGLE:
        case RECTANGLE:
            rad = penSize[penType];
            update(QRectF(
                last_begin, last_end
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            update(QRectF(
                startPoint, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length() + penSize[penType];
            frad = QLineF(last_begin, last_end).length() + penSize[penType];
            update(QRectF(
                startPoint,startPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            update(QRectF(
                startPoint,startPoint
            ).toRect().normalized().adjusted(-frad, -frad, +frad, +frad));
            break;
     }

    last_begin = startPoint;
    last_end = endPoint;

    painter.end();

}
#ifdef LIBARCHIVE
void DrawingWidget::saveAll(QString file){
    if (!file.isEmpty()) {
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
    img = img.scaled(mainWindow->geometry().width(), mainWindow->geometry().height());
    if(img.isNull()){
        return;
    }
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
                floatingSettings->hide();
            }
            QTouchEvent *touchEvent = static_cast<QTouchEvent*>(ev);
            QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->points();
            foreach(const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
                QPointF pos = touchPoint.position();
                if ((Qt::TouchPointState)touchPoint.state() == Qt::TouchPointPressed) {
                    storage.saveValue(touchPoint.id(), pos);
                }
                else if ((Qt::TouchPointState)touchPoint.state() == Qt::TouchPointReleased) {
                    curs.hide(touchPoint.id());
                    storage.saveValue(touchPoint.id(), QPointF(-1,-1));
                    continue;
                }
                QPointF oldPos = storage.loadValue(touchPoint.id());
                drawLineToFunc(oldPos.toPoint(), pos.toPoint(), touchPoint.pressure());
                storage.saveValue(touchPoint.id(), pos);
                updateCursorMouse(touchPoint.id(), pos.toPoint());
                curs.setCursor(touchPoint.id(), penSize[penType]);
            }
            break;
        }
        case QEvent::TabletPress: {
            QTabletEvent *tabletEvent = static_cast<QTabletEvent*>(ev);
            lastPoint = tabletEvent->position();
            firstPoint = tabletEvent->position();
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
            QPointF pos = tabletEvent->position();
            drawLineToFunc(lastPoint, pos.toPoint(), tabletEvent->pressure());
            lastPoint = tabletEvent->position();
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

