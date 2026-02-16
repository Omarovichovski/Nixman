#ifndef DEPLOYER_H
#define DEPLOYER_H

#include "Device.h"
#include "ConfigState.h"

class Deployer {
public:
    static bool deploy(const Device& device, const ConfigState& state);
};

#endif
