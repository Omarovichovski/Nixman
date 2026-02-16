#include "Inventory.h"
#include <fstream>
#include <nlohmann/json.hpp>

void Inventory::addDevice(const Device& d) {
    devices.push_back(d);
}

void Inventory::removeDevice(const std::string& name) {
    devices.erase(
        std::remove_if(devices.begin(), devices.end(),
        [&name](const Device& d) { return d.getName() == name; }),
        devices.end()
    );
}

const std::vector<Device>& Inventory::getDevices() const {
    return devices;
}

bool Inventory::saveToFile(const std::string& filename) const {
    nlohmann::json j;
    for (const auto& d : devices) {
        j.push_back(d.to_json());
    }
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << j.dump(4);
    return true;
}

bool Inventory::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    nlohmann::json j;
    file >> j;
    devices.clear();
    for (const auto& item : j) {
        devices.push_back(Device::from_json(item));
    }
    return true;
}
