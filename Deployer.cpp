#include "Deployer.h"
#include "ConfigGenerator.h"
#include "FileUtils.h"
#include <cstdlib>
#include <iostream>

bool Deployer::deploy(const Device& device,
                      const ConfigState& state)
{
    std::cout << "Deploying to: "
              << device.getName() << "\n";
    std::string config = ConfigGenerator::generateConfig(device, state);

    std::string localFile =
        "./generated-" + device.getName() + ".nix";

    if (!FileUtils::writeToFile(localFile, config)) {
        std::cerr << "Failed to write config file\n";
        return false;
    }

    std::string sshTarget =
        device.getUsername() + "@" + device.getIP();

    std::string command =
        "nixos-rebuild switch "
        "--target-host " + sshTarget + " "
        "-I nixos-config=" + localFile + " "
        "--use-remote-sudo "
        "-i " + device.getSSHKeyPath();

    std::cout << "Executing:\n" << command << "\n";

    int result = std::system(command.c_str());

    return result == 0;
}
