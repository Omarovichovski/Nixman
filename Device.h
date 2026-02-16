#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <nlohmann/json.hpp>

class Device {
private:
    std::string name;
    std::string ip;
    std::string username;
    std::string ssh_key_path;

public:
    Device() = default;
    Device(const std::string& name, const std::string& ip,
           const std::string& username, const std::string& ssh_key_path);

    std::string getName() const;
    std::string getIP() const;
    std::string getUsername() const;
    std::string getSSHKeyPath() const;

    nlohmann::json to_json() const;
    static Device from_json(const nlohmann::json& j);
};

#endif