#ifndef _ARCHIVE_H

#ifdef LIBARCHIVE
extern "C" {
    extern void *load_archive(void* arg);
    extern QString archive_target;
}
#endif
#define _ARCHIVE_H
#include <QImage>
#include <QString>
void archive_create(const QString& archiveFileName, const QString& cfg);
void archive_add(const QString& path, const QImage& image);
void archive_add_file(const QString& path, const QString& file);
void archive_finish();
void archive_load(const QString& archiveFileName);
bool archive_next(QString& path, QImage& image);
QString archive_get_config();

#endif
