#include "Device.h"

Device::Device(const std::string& name, const std::string& ip,
               const std::string& username, const std::string& ssh_key_path)
    : name(name), ip(ip), username(username), ssh_key_path(ssh_key_path) {}

std::string Device::getName() const { return name; }
std::string Device::getIP() const { return ip; }
std::string Device::getUsername() const { return username; }
std::string Device::getSSHKeyPath() const { return ssh_key_path; }

nlohmann::json Device::to_json() const {
    return nlohmann::json{
        {"name", name},
        {"ip", ip},
        {"username", username},
        {"ssh_key_path", ssh_key_path}
    };
}

Device Device::from_json(const nlohmann::json& j) {
    return Device(
        j.at("name").get<std::string>(),
        j.at("ip").get<std::string>(),
        j.at("username").get<std::string>(),
        j.at("ssh_key_path").get<std::string>()
    );
}
