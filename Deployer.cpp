#include "Deployer.h"
#include <thread>
#include <vector>
#include <mutex>

std::map<std::string, bool> Deployer::deployAll(
    const Inventory& inventory,
    ConfigGenerator& generator,
    SSHManager& ssh,
    const ConfigState& state
) {
    std::map<std::string, bool> results;
    std::mutex resultMutex;
    std::vector<std::thread> threads;

    for (const auto& device : inventory.getDevices()) {
        threads.emplace_back([&device, &generator, &ssh, &results, &resultMutex, &state]() {

            std::string config = generator.generateConfig(device, state);
            bool success = ssh.deployConfig(device, config);

            std::lock_guard<std::mutex> lock(resultMutex);
            results[device.getName()] = success;
        });
    }

    for (auto& t : threads)
        t.join();

    return results;
}
