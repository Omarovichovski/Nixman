#ifndef NIXOPTION_H
#define NIXOPTION_H

#include <string>

struct NixOption {
    std::string name;
    std::string type;
    std::string description;
    std::string defaultValue;
};

#endif
