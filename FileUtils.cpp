#include "FileUtils.h"
#include <fstream>

bool FileUtils::writeToFile(const std::string& path, const std::string& content) {
    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << content;
    out.close();
    return true;
}
