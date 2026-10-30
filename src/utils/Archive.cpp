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

#include <constants.h>
#include <widgets/DrawingWidget.h>

extern QWidget* mainWidget;

extern QString cache;

class ArchiveStorage {
public:
    QString config = "";

    // Entries go to the file as soon as they are added. A saved file can hold
    // hundreds of full screen frames, keeping them all in memory until the end
    // takes gigabytes at 4K.
    void create(const QString& archiveFileName, const QString& cfg) {
        // Open the archive file
        writer = archive_write_new();
        archive_write_add_filter_gzip(writer);
        archive_write_set_format_pax_restricted(writer);
        archive_write_open_filename(writer, archiveFileName.toStdString().c_str());
        // write config
        struct archive_entry* entry = archive_entry_new();
        archive_entry_set_pathname(entry, "config");
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_entry_set_size(entry, cfg.size());
        archive_write_header(writer, entry);
        archive_write_data(writer, cfg.toStdString().c_str(), cfg.size());
        archive_entry_free(entry);
    }

    void add_file(const QString& path, const QString& file) {
        struct stat st;
        char buff[8192];
        int len;
        int fd;

        struct archive_entry* entry = archive_entry_new();
        stat(file.toStdString().c_str(), &st);
        printf("Compress:%s => %s\n", path.toStdString().c_str(), file.toStdString().c_str());
        archive_entry_set_pathname(entry, path.toStdString().c_str());
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_entry_set_size(entry, st.st_size);
        archive_write_header(writer, entry);
        fd = open(file.toStdString().c_str(), O_RDONLY);
        if (fd < 0) {
            archive_entry_free(entry);
            return;
        }
        len = read(fd, buff, sizeof(buff));
        while ( len > 0 ) {
            archive_write_data(writer, buff, len);
            len = read(fd, buff, sizeof(buff));
        }
        close(fd);
        archive_entry_free(entry);
    }

    void add(const QString& path, const QImage& image) {
        // Convert QImage to a QByteArrayn
        QByteArray imageData = QByteArray::fromRawData(reinterpret_cast<const char*>(image.constBits()), image.sizeInBytes());
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
        archive_write_header(writer, entry);
        archive_write_data(writer, byteArray.data(), byteArray.size());
        archive_entry_free(entry);
    }

    void finish() {
        // Clean up
        archive_write_close(writer);
        archive_write_free(writer);
        writer = NULL;
    }

    void load(const QString& archiveFileName) {
        config = "";
        // Open the archive file
        reader = archive_read_new();
        archive_read_support_filter_all(reader);
        archive_read_support_format_all(reader);
        int r = archive_read_open_filename(reader, archiveFileName.toStdString().c_str(), 10240); // 10240 is the block size
        if (r != ARCHIVE_OK) {
            printf("Failed to open archive: %s", archive_error_string(reader));
            archive_read_free(reader);
            reader = NULL;
        }
    }

    // Returns the images one by one, config and overlay.pdf are handled here.
    bool next(QString& path, QImage& result) {
        if (!reader) {
            return false;
        }
        struct archive_entry *entry;
        int width, height, format;
        while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
            // Get entry name
            const char* entryName = archive_entry_pathname(entry);
            // Check if it's an image file
            if (entryName) {
                // Extract the image data
                int64_t size = archive_entry_size(entry);
                if(size <= 0){
                    continue;
                }
                QByteArray input;
                while (1) {
                    char buf[4096];
                    long int r = archive_read_data(reader, buf, sizeof(buf));
#ifdef __LP64__
                    debug("%ld %ld %s\n", r, size, entryName);
#else
                    debug("%ld %lld %s\n", r, size, entryName);
#endif
                    if (r > 0){ // continue read
                        input.append(buf, r);
                    } else if (r == 0) { // done
                        break;
                    } else { // error
                        input.clear();
                        printf("Failed to read: %s\n", entryName);
                        break;
                    }
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
                path = QString(entryName);
                result = image.copy();
                return true;
            }
        }
        // Close the archive
        archive_read_close(reader);
        archive_read_free(reader);
        reader = NULL;
        return false;
    }

private:
    struct archive* writer = NULL;
    struct archive* reader = NULL;
};

ArchiveStorage archive;

void archive_create(const QString& archiveFileName, const QString& cfg){
    archive.create(archiveFileName, cfg);
}
void archive_add(const QString& path, const QImage& image){
    archive.add(path, image);
}
void archive_add_file(const QString& path, const QString& file){
    archive.add_file(path, file);
}
void archive_finish(){
    archive.finish();
}

void archive_load(const QString& archiveFileName){
    archive.load(archiveFileName);
}
bool archive_next(QString& path, QImage& image){
    return archive.next(path, image);
}
QString archive_get_config(){
    return archive.config;
}
