#ifndef DEPLOYER_H
#define DEPLOYER_H

#include "Inventory.h"
#include "ConfigGenerator.h"
#include "SSHManager.h"
#include "ConfigState.h"
#include <map>

class Deployer {
public:
    std::map<std::string, bool> deployAll(
        const Inventory& inventory,
        ConfigGenerator& generator,
        SSHManager& ssh,
        const ConfigState& state
    );
};

#endif
