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

extern WhiteBoard *board;

extern DrawingWidget *window;

extern void updateGoBackButtons();
void removeDirectory(const QString &path);

int screenWidth = 0;
int screenHeight = 0;
int padding = 8;


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

class ImageStorage {
public:
    int last_image_num = 0;
    int image_count = 0;
    int pageType = TRANSPARENT;
    int overlayType = NONE;
    void saveValue(qint64 id, QImage data) {
        values[id] = data;
        updateGoBackButtons();
    }

    void clear(){
        values.clear();
        image_count = 0;
        last_image_num = 0;
        updateGoBackButtons();
    }

    QImage loadValue(qint64 id) {
        if (values.contains(id)) {
            return values[id];
        } else {
            QImage image = QImage(screenWidth,screenHeight, QImage::Format_ARGB32);
            image.fill(QColor("transparent"));
            return image;
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
        values[last_page_num] = images;
        for(int i=0;i<=page_count;i++){
            for(int j=1;j<=loadValue(i).image_count;j++){
                archive_add(QString::number(i)+"/"+QString::number(j-1), values[i].loadValue(j));
            }
        }
        archive_create(filename);
    }

    void loadArchive(const QString& filename){
        QMap<QString, QImage> archive = archive_load(filename);
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
        images = values[0];
        window->loadImage(images.last_image_num);
        window->update();
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

DrawingWidget::DrawingWidget(QWidget *parent): QWidget(parent) {
    initializeImage(size());
    penType = 1;
    QScreen *screen = QGuiApplication::primaryScreen();
    screenWidth  = screen->geometry().width();
    screenHeight = screen->geometry().height();
    setFixedSize(screenWidth, screenHeight);
    padding = screenWidth / 240;
}

DrawingWidget::~DrawingWidget() {}

void DrawingWidget::mousePressEvent(QMouseEvent *event) {
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

void DrawingWidget::mouseMoveEvent(QMouseEvent *event) {
    int penTypeBak = penType;
    if(event->buttons() & Qt::RightButton) {
        penType = ERASER;
    }else if(event->buttons() & Qt::MiddleButton) {
        penType = MARKER;
    }
    if (drawing) {
        drawLineTo(event->position());
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
}

void DrawingWidget::initializeImage(const QSize &size) {
    image = QImage(size, QImage::Format_ARGB32);
    image.fill(QColor("transparent"));
}

void DrawingWidget::resizeEvent(QResizeEvent *event) {
    initializeImage(event->size());
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

void DrawingWidget::drawLineTo(const QPointF &endPoint) {
    drawLineToFunc(lastPoint, endPoint, 1.0);
    lastPoint = endPoint;
}

void DrawingWidget::drawLineToFunc(QPointF startPoint, QPointF endPoint, qreal pressure) {
    if(startPoint.x() < 0 || startPoint.y() < 0){
        return;
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

    painter.setPen(QPen(penColor, (penSize[penType]*pressure*screenHeight)/1080, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    switch(fpenStyle){
        case SPLINE:
            rad = (penSize[penType]*pressure*screenHeight)/1080;
            painter.drawLine(startPoint, endPoint);
            update(QRectF(
                startPoint, endPoint
            ).toRect().normalized().adjusted(-rad, -rad, +rad, +rad));
            break;
        case LINE:
            painter.drawLine(startPoint, endPoint);
            update();
            break;
        case CIRCLE:
            rad = QLineF(startPoint, endPoint).length();
            painter.drawEllipse(startPoint, rad, rad);
            update();
            break;
    }

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
    if(img.isNull()){
        return;
    }
    QPainter p(&image);
    QRectF target(0, 0, screenWidth, screenHeight);
    QRectF source(0.0, 0.0, screenWidth, screenHeight);
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
                    storage.saveValue(touchPoint.id(), QPointF(-1,-1));
                    continue;
                }
                QPointF oldPos = storage.loadValue(touchPoint.id());
                drawLineToFunc(oldPos.toPoint(), pos.toPoint(), touchPoint.pressure());
                storage.saveValue(touchPoint.id(), pos);
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
            QPointF pos = tabletEvent->position();
            drawLineToFunc(lastPoint, pos.toPoint(), tabletEvent->pressure());
            lastPoint = tabletEvent->position();
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
    return images.last_image_num > 0;
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

