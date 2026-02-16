#ifndef INVENTORY_H
#define INVENTORY_H

#include "Device.h"
#include <vector>
#include <string>

class Inventory {
private:
    std::vector<Device> devices;

public:
    void addDevice(const Device& d);
    void removeDevice(const std::string& name);
    const std::vector<Device>& getDevices() const;

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
};

#endif
