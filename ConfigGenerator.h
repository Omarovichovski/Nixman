#ifndef CONFIGGENERATOR_H
#define CONFIGGENERATOR_H

#include "Device.h"
#include "ConfigState.h"
#include <string>

class ConfigGenerator {
public:
    static std::string generateConfig(const Device& d, const ConfigState& state);
};

#endif
