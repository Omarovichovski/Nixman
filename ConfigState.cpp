#include "ConfigState.h"
#include <sstream>

// ==================== PACKAGE MANAGEMENT ====================
void ConfigState::addPackage(const std::string& pkgName, const std::map<std::string, std::string>& options) {
    packages[pkgName] = options;
}

void ConfigState::removePackage(const std::string& pkgName) {
    packages.erase(pkgName);
}

const std::map<std::string, std::map<std::string, std::string>>& ConfigState::getPackages() const {
    return packages;
}

const std::map<std::string, std::string>& ConfigState::getPackageOptions(const std::string& pkgName) const {
    static std::map<std::string, std::string> empty;
    auto it = packages.find(pkgName);
    if (it != packages.end()) return it->second;
    return empty;
}

// ==================== USERS ========================

std::string ConfigState::getUsers() const {
    std::ostringstream out;
    bool first = true;
    for (const auto& [uname, _] : usersMap) {
        if (!first) out << ",";
        out << uname;
        first = false;
    }
    return out.str();
}
