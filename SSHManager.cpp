#include "SSHManager.h"
#include <fstream>
#include <cstdlib>

bool SSHManager::deployConfig(const Device& d, const std::string& config) {
    // Save config to temporary file
    std::string tempFile = "/tmp/" + d.getName() + "_config.nix";
    std::ofstream out(tempFile);
    if (!out.is_open()) return false;
    out << config;
    out.close();

    // Construct SSH command to copy file and apply it
    std::string cmd = 
        "scp -i " + d.getSSHKeyPath() + " " + tempFile + " " +
        d.getUsername() + "@" + d.getIP() + ":~/configuration.nix && " +
        "ssh -i " + d.getSSHKeyPath() + " " +
        d.getUsername() + "@" + d.getIP() + " " +
        "\"sudo nixos-rebuild switch -I nixos-config=~/configuration.nix\"";

    int result = system(cmd.c_str());

    // Optionally, remove temporary file
    std::remove(tempFile.c_str());

    return result == 0;
}
