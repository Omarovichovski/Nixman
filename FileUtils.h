#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>

namespace FileUtils {
    bool writeToFile(const std::string& path, const std::string& content);
}

#endif