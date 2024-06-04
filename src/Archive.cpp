#include <QImage>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QIODevice>
#include <QDebug>
#include <archive.h>
#include <archive_entry.h>

extern int screenWidth;
extern int screenHeight;

class ArchiveStorage {
public:
    void add(const QString& path, const QImage& image) {
        values[path] = image;
    }

    void create(const QString& archiveFileName) {
        // Open the archive file
        struct archive* ar = archive_write_new();
        archive_write_add_filter_gzip(ar);
        archive_write_set_format_pax_restricted(ar);
        archive_write_open_filename(ar, archiveFileName.toStdString().c_str());
        // write config
        struct archive_entry* entry = archive_entry_new();
        QString config = QString::number(screenWidth) + "x" + QString::number(screenHeight);
        archive_entry_set_pathname(entry, "config");
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_entry_set_size(entry, config.size());
        archive_write_header(ar, entry);
        archive_write_data(ar, config.toStdString().c_str(), config.size());
        archive_entry_free(entry);
        
        for (auto it = values.begin(); it != values.end(); ++it) {
            QString path = it.key();
            QImage image = it.value();
            // Convert QImage to a QByteArray
            QByteArray imageData(reinterpret_cast<const char*>(image.constBits()), image.sizeInBytes());
            // Create an entry and write image data to the archive
            struct archive_entry* entry = archive_entry_new();
            printf("Compress:%s\n", path.toStdString().c_str());
            archive_entry_set_pathname(entry, path.toStdString().c_str());
            archive_entry_set_filetype(entry, AE_IFREG);
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_size(entry, imageData.size());
            archive_write_header(ar, entry);
            archive_write_data(ar, imageData.data(), imageData.size());
            archive_entry_free(entry);
        }
        // Clean up
        archive_write_close(ar);
        archive_write_free(ar);
    }

    QMap<QString, QImage> load(const QString& archiveFileName) {
        QMap<QString, QImage> values;
        // Open the archive file
        struct archive *ar;
        struct archive_entry *entry;
        ar = archive_read_new();
        archive_read_support_filter_all(ar);
        archive_read_support_format_all(ar);
        int r = archive_read_open_filename(ar, archiveFileName.toStdString().c_str(), 10240); // 10240 is the block size
        if (r != ARCHIVE_OK) {
            qDebug() << "Failed to open archive: " << archive_error_string(ar);
            return values;
        }
        int width = screenWidth;
        int height = screenHeight;
        while (archive_read_next_header(ar, &entry) == ARCHIVE_OK) {
            // Get entry name
            const char* entryName = archive_entry_pathname(entry);
            // Check if it's an image file (you may need to modify this condition)
            if (entryName) {
                // Extract the image data
                QByteArray *imageData = new QByteArray();
                char buff[10240];
                size_t size;
                size_t total_size = 0;
                while ((size = archive_read_data(ar, buff, sizeof(buff))) > 0) {
                    if(size > 10240){
                        break;
                    }
                    // printf("Read: %ld bytes\n", size);
                    imageData->append(buff, size);
                    total_size+= size;
                }
                printf("Decompress:%s %ld\n", entryName, total_size);
                if(strcmp(entryName, "config") == 0){
                    QStringList res = QString::fromUtf8(*imageData).split("x");
                    width = res[0].toInt();
                    height = res[1].toInt();
                    continue;
                }
                QImage image = QImage(reinterpret_cast<const uchar*>(imageData->data()), width, height, QImage::Format_ARGB32);
                if (image.isNull()) {
                    puts("Image load fail");
                    continue;
                }
                image = image.scaled(screenWidth, screenHeight);
                values.insert(QString(entryName), image);
            } else {
                break;
            }
        }
        // Close the archive
        archive_read_close(ar);
        archive_read_free(ar);
        return values;
    }

private:
    QMap<QString, QImage> values;
};

ArchiveStorage archive;

void archive_add(const QString& path, const QImage& image){
    archive.add(path, image);
}

void archive_create(const QString& archiveFileName){
    archive.create(archiveFileName);
}

QMap<QString, QImage> archive_load(const QString& archiveFileName) {
    return archive.load(archiveFileName);
}
