#include "Archive.h"
#include <archive.h>
#include <archive_entry.h>
#include <cstdio>
#include <iostream>

#include <QtWidgets>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ArchiveCreator::ArchiveCreator() {
    a = archive_write_new();
    archive_write_add_filter_gzip(a);
    archive_write_set_format_pax_restricted(a);
}

ArchiveCreator::~ArchiveCreator() {
    if (a) {
        archive_write_free(a);
    }
}

int ArchiveCreator::addPath(const std::string& filePath, const std::string& entryName) {
    struct archive_entry* entry = archive_entry_new();
    archive_entry_set_pathname(entry, entryName.c_str());
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0644);

    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        archive_entry_free(entry);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    archive_entry_set_size(entry, fileSize);

    int ret = archive_write_header(a, entry);
    if (ret != ARCHIVE_OK) {
        fclose(file);
        archive_entry_free(entry);
        return ret;
    }

    char buff[8192];
    size_t len;
    int fd = open(filePath.c_str(), O_RDONLY);
    len = read(fd, buff, sizeof(buff));
    while ( len > 0 ) {
        archive_write_data(a, buff, len);
        len = read(fd, buff, sizeof(buff));
    }
    close(fd);

    fclose(file);
    archive_entry_free(entry);
    return 0;
}

int ArchiveCreator::create(const std::string& archivePath) {
    int ret = archive_write_open_filename(a, archivePath.c_str());
    if (ret != ARCHIVE_OK) {
        return ret;
    }
    return 0;
}

int ArchiveCreator::closePath() {
    int ret = archive_write_close(a);
    if (ret != ARCHIVE_OK) {
        return ret;
    }
    return 0;
}

int ArchiveCreator::extract(const std::string& archivePath, const std::string& targetPath) {
    struct archive *a;
    struct archive_entry *entry;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    
    r = archive_read_open_filename(a, archivePath.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        std::cerr << "Failed to open archive: " << archive_error_string(a) << std::endl;
        return 1;
    }

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        // Get the entry path within the archive
        const char* entryPath = archive_entry_pathname(entry);
        mode_t mode = archive_entry_filetype(entry);
        if (!entryPath) {
            // Some entries might not have a path
            continue;
        }
        
        // Construct the target path where the entry will be extracted
        std::string targetFilePath = targetPath + "/" + entryPath;
        if (S_ISDIR(mode)) {
            continue;
        }
        QFileInfo fileInfo(QString::fromStdString(targetFilePath));
        QString dirname = fileInfo.dir().absolutePath();
        qDebug() << "Directory" << dirname;
        QDir dir(dirname);
        if (!dir.exists(dirname)) {
            dir.mkpath(dirname);
        }
        // Extract the entry to the target path
        std::cout << "Extracting: " << targetFilePath << std::endl;
        FILE *file = fopen(targetFilePath.c_str(), "wb");
		if (file == NULL) {
		    printf("Failed to open file for writing: %s\n", targetFilePath.c_str());
		    return 1;
		}
		char buffer[4096];
		ssize_t size;
		while ((size = archive_read_data(a, buffer, sizeof(buffer))) > 0) {
		    fwrite(buffer, 1, size, file);
		}
		fclose(file);
    }

    archive_read_close(a);
    archive_read_free(a);
    return 0;
}


