#ifndef NIXPACKAGE_H
#define NIXPACKAGE_H

#include <string>
#include <map>

struct NixPackage {
    std::string name;
    std::string description;
    bool selected = false;
    std::map<std::string, std::string> options; // package/module options
    std::string moduleName;                     // auto-detected NixOS module prefix
};

#endif
