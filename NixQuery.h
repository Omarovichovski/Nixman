#ifndef NIXQUERY_H
#define NIXQUERY_H

#include <vector>
#include <string>
#include "NixPackage.h"
#include "NixOption.h"

class NixQuery {
public:
    std::vector<NixPackage> search(const std::string& term);
    std::vector<NixOption> queryModuleOptions(const std::string& prefix);
    std::string detectModulePrefix(const std::string& pkgName);
    bool moduleExists(const std::string& modulePrefix);
    std::vector<NixOption> getPackageOptions(const std::string& name, bool alreadyModule=false);
};

#endif
