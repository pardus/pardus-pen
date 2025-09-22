#include <QImage>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <archive.h>
#include <archive_entry.h>
#include <QWidget>

#include <fcntl.h>

#include "../tools.h"

extern QWidget* mainWidget;

extern QString cache;

class ArchiveStorage {
public:
    QString config = "";
    void add(const QString& path, const QImage& image) {
        values[path] = image;
    }

    void add_file(const QString& path, const QString& file) {
        files[path] = file;
    }

    void setConfig(QString cfg){
        config = cfg;
    }

    void create(const QString& archiveFileName) {
        // Open the archive file
        struct archive* ar = archive_write_new();
        archive_write_add_filter_gzip(ar);
        archive_write_set_format_pax_restricted(ar);
        archive_write_open_filename(ar, archiveFileName.toStdString().c_str());
        // write config
        struct archive_entry* entry = archive_entry_new();
        archive_entry_set_pathname(entry, "config");
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_entry_set_size(entry, config.size());
        archive_write_header(ar, entry);
        archive_write_data(ar, config.toStdString().c_str(), config.size());
        archive_entry_free(entry);

        struct stat st;
        char buff[8192];
        int len;
        int fd;


        for (auto it = files.begin(); it != files.end(); ++it) {
            struct archive_entry* entry = archive_entry_new();
            QString path = it.key();
            QString file = it.value();
            stat(file.toStdString().c_str(), &st);
            printf("Compress:%s => %s\n", path.toStdString().c_str(), file.toStdString().c_str());
            archive_entry_set_pathname(entry, path.toStdString().c_str());
            archive_entry_set_filetype(entry, AE_IFREG);
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_size(entry, st.st_size);
            archive_write_header(ar, entry);
            fd = open(file.toStdString().c_str(), O_RDONLY);
            len = read(fd, buff, sizeof(buff));
            while ( len > 0 ) {
                archive_write_data(ar, buff, len);
                len = read(fd, buff, sizeof(buff));
            }
            close(fd);
            archive_entry_free(entry);
        }

        for (auto it = values.begin(); it != values.end(); ++it) {
            QString path = it.key();
            QImage image = it.value();
            // Convert QImage to a QByteArrayn
            QByteArray imageData(reinterpret_cast<const char*>(image.constBits()), image.sizeInBytes());
            QByteArray byteArray;
            QDataStream out(&byteArray, QIODevice::WriteOnly);
            out << image.width() << image.height() << static_cast<int>(image.format()) << imageData;

            // Create an entry and write image data to the archive
            struct archive_entry* entry = archive_entry_new();
            printf("Compress:%s\n", path.toStdString().c_str());
            archive_entry_set_pathname(entry, path.toStdString().c_str());
            archive_entry_set_filetype(entry, AE_IFREG);
            archive_entry_set_perm(entry, 0644);
            archive_entry_set_size(entry, byteArray.size());
            archive_write_header(ar, entry);
            archive_write_data(ar, byteArray.data(), byteArray.size());
            archive_entry_free(entry);
        }
        // Clean up
        archive_write_close(ar);
        archive_write_free(ar);
        values.clear();
    }

    QMap<QString, QImage> load(const QString& archiveFileName) {
        QMap<QString, QImage> values;
        config = "";
        // Open the archive file
        struct archive *ar;
        struct archive_entry *entry;
        ar = archive_read_new();
        archive_read_support_filter_all(ar);
        archive_read_support_format_all(ar);
        int r = archive_read_open_filename(ar, archiveFileName.toStdString().c_str(), 10240); // 10240 is the block size
        if (r != ARCHIVE_OK) {
            printf("Failed to open archive: %s", archive_error_string(ar));
            return values;
        }

        int width, height, format;
        while (archive_read_next_header(ar, &entry) == ARCHIVE_OK) {
            // Get entry name
            const char* entryName = archive_entry_pathname(entry);
            // Check if it's an image file
            if (entryName) {
                // Extract the image data
                QByteArray input;
                char buff[10240];
                size_t size;
                while ((size = archive_read_data(ar, buff, sizeof(buff))) > 0) {
                    input.append(buff, size);
                }
#ifdef QPRINTER
                if (strcmp(entryName, "overlay.pdf") == 0) {
                    drawing->pdfPath = cache + generateRandomString(10);
                    QDir dir; dir.mkpath(cache);
                    QFile newDoc(drawing->pdfPath);
                    if(newDoc.open(QIODevice::WriteOnly)){
                        newDoc.write(input);
                    }
                    newDoc.close();
                    loadPdfFromData(input);
                    continue;
                }
#endif
                if (strcmp(entryName, "config") == 0) {
                    config = QString::fromUtf8(input);
                    QStringList list = config.split("\n");
                    for (const auto &str : list) {
                        if (str.startsWith("width=")) {
                            width = str.split("=")[1].toInt();
                        } else if (str.startsWith("height=")) {
                            height = str.split("=")[1].toInt();
                        }
                    }
                    continue;
                }

                // Read image data from QByteArray
                QByteArray loadedData;
                QDataStream inputStream(&input, QIODevice::ReadOnly);
                inputStream >> width >> height >> format >> loadedData;

                QImage image(reinterpret_cast<const uchar*>(loadedData.constData()), width, height, static_cast<QImage::Format>(format));

                if (image.isNull()) {
                    printf("Image load fail: %s\n", entryName);
                    continue;
                }
                values.insert(QString(entryName), image.copy());
            }
        }
        // Close the archive
        archive_read_close(ar);
        archive_read_free(ar);
        return values;
    }

private:
    QMap<QString, QImage> values;
    QMap<QString, QString> files;
};

ArchiveStorage archive;

void archive_add(const QString& path, const QImage& image){
    archive.add(path, image);
}
void archive_add_file(const QString& path, const QString& file){
    archive.add_file(path, file);
}

void archive_set_config(const QString& cfg){
    archive.setConfig(cfg);
}
QString archive_get_config(){
    return archive.config;
}

void archive_create(const QString& archiveFileName){
    archive.create(archiveFileName);
}

QMap<QString, QImage> archive_load(const QString& archiveFileName) {
    return archive.load(archiveFileName);
}
