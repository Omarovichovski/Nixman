#ifndef SSHMANAGER_H
#define SSHMANAGER_H

#include "Device.h"
#include <string>

class SSHManager {
public:
    bool deployConfig(const Device& d, const std::string& config);
};

#endif
