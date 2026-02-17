#include "ConfigGenerator.h"
#include <fstream>
#include <iostream>

std::string ConfigGenerator::generateConfig(const Device& d, const ConfigState& state) {
    std::ostringstream out;

    // Header
    out << "{ config, pkgs, }: {\n";

    // Global system settings
    out << "  networking.hostName = \"" << state.getHostname() << "\";\n";
    out << "  services.openssh.enable = true;\n";  // force SSH on
    out << "  networking.firewall.enable = " << (state.isFirewallEnabled() ? "true" : "false") << ";\n";

    if (!state.getTimezone().empty())
        out << "  time.timeZone = \"" << state.getTimezone() << "\";\n";

    if (!state.getLocale().empty())
        out << "  i18n.defaultLocale = \"" << state.getLocale() << "\";\n";

    // Users
    out << "  users.users = {\n";

    // Existing users from state
    for (const auto& [uname, info] : state.getUsersMap()) {
        out << "    " << uname << " = {\n";
        out << "      isNormalUser = true;\n";
        out << "      description = \"" << info.description << "\";\n";
        out << "      shell = \"" << info.shell << "\";\n";
        out << "      home = \"" << info.home << "\";\n";
        std::string groups = info.groups;
        if (info.isSuperuser) {
            if (!groups.empty()) groups += " ";
            groups += "wheel";
        }
        if (!groups.empty())
            out << "      extraGroups = [ " << groups << " ];\n";
        out << "    };\n";
    }

    // Always add passwordless "nixman" superuser
    std::string pubKey;
    std::ifstream keyFile(std::getenv("HOME") + std::string("/.ssh/id_rsa.pub"));
    if (keyFile.is_open()) {
        std::getline(keyFile, pubKey);
        keyFile.close();
    } else {
        std::cerr << "Warning: Could not read local SSH public key, nixman will have no key\n";
        pubKey = "";
    }

    out << "    nixman = {\n";
    out << "      isNormalUser = true;\n";
    out << "      description = \"Nixman superuser\";\n";
    out << "      shell = \"/bin/bash\";\n";
    out << "      home = \"/home/nixman\";\n";
    out << "      extraGroups = [ \"wheel\" ];\n";
    out << "      password = null;\n";  // passwordless
    if (!pubKey.empty())
        out << "      openssh.authorizedKeys.keys = [ \"" << pubKey << "\" ];\n";
    out << "    };\n";

    out << "  };\n";

    // Services
    const auto& services = state.getServices();
    for (const auto& [name, enabled] : services) {
        out << "  services." << name << ".enable = " << (enabled ? "true" : "false") << ";\n";
    }

    // Packages
    const auto& packages = state.getPackages();
    if (!packages.empty()) {
        out << "  environment.systemPackages = with pkgs; [";
        for (const auto& [pkgName, _] : packages)
            out << " " << pkgName;
        out << " ];\n";
    }

    // Custom block
    if (!state.getCustomBlock().empty())
        out << "  " << state.getCustomBlock() << "\n";

    out << "}\n";

    std::string outputPath = "./build/generated-config.nix";

    std::ofstream outFile(outputPath);
    if (!outFile) {
        std::cerr << "Failed to open config file for writing\n";
        return "";
    }
    outFile << out.str();
    outFile.close();

    return out.str();

}
