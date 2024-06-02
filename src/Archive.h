#ifndef ARCHIVE_CREATOR_H
#define ARCHIVE_CREATOR_H

#include <archive.h>
#include <archive_entry.h>
#include <string>

class ArchiveCreator {
private:
    struct archive *a;

public:
    ArchiveCreator();
    ~ArchiveCreator();

    int addPath(const std::string& filePath, const std::string& entryName);
    int create(const std::string& archivePath);
    int closePath();
    int extract(const std::string& archivePath, const std::string& targetPath);
};

#endif // ARCHIVE_CREATOR_H

