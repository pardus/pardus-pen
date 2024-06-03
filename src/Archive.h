#ifndef _ARCHIVE_H
#define _ARCHIVE_H
#include <QImage>
#include <QString>
void archive_add(const QString& path, const QImage& image);
void archive_create(const QString& archiveFileName);
QMap<QString, QImage> archive_load(const QString& archiveFileName) ;

#endif
