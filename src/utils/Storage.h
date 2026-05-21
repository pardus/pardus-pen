#ifndef STORAGE_H
#define STORAGE_H

#include <QMap>
#include <QPointF>
#include <QImage>
#include <QString>

class QLabel;
class QWidget;
class QVBoxLayout;

class ValueStorage {
public:
    int size();
    void clear();
    void saveValue(qint64 id, QPointF data);
    QPointF last();
    QPointF first();
    QPointF loadValue(qint64 id);
    QMap<qint64, QPointF> values;
};

class GeometryStorage {
public:
    QPointF point;
    QPointF point_last;

    void addValue(qint64 id, QPointF data);
    void saveValue(qint64 id, qint64 id2, QPointF data);
    QPointF last(qint64 id);
    QPointF first(qint64 id);
    int size(qint64 id);
    ValueStorage load(qint64 id);
    void clear(qint64 id);
    void clearAll();
    QMap<qint64, ValueStorage> values;
    QMap<qint64, QPointF> last_begin;
    QMap<qint64, QPointF> last_end;
};

class CursorStorage {
public:
    void init(qint64 id);
    void setPosition(qint64 id, QPointF data);
    void setCursor(qint64 id, int size);
    void clear();
    void hide(qint64 id);

    QMap<qint64, bool> drawing;
    QMap<qint64, int> penType;
    QMap<qint64, QWidget*> images;
    QMap<qint64, QLabel*> labels;
    QMap<qint64, QVBoxLayout*> layouts;
    QMap<qint64, int> sizes;
};

class ImageStorage {
public:
    int last_image_num = 1;
    int image_count = 0;
    int pageType = 30;
    int overlayType = 40;
    int removed = 0;

    ImageStorage();
    void saveValue(qint64 id, QImage data);
    void clear();
    QImage loadValue(qint64 id);
    void remove(qint64 id);

private:
    QString cache_path;
};

class PageStorage {
public:
    int last_page_num = 0;
    int page_count = 0;

    void saveValue(qint64 id, ImageStorage data);
    void clear();
    ImageStorage loadValue(qint64 id);

#ifdef QPRINTER
    void savePdf(const QString& filename);
#endif

#ifdef LIBARCHIVE
    void saveAll(const QString& filename);
    void loadArchive(const QString& filename);
#endif

private:
    QMap<qint64, ImageStorage> values;
};

extern CursorStorage curs;
extern ImageStorage images;
extern PageStorage pages;

#endif
