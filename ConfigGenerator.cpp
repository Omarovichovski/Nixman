#include "ConfigGenerator.h"
#include <sstream>

std::string ConfigGenerator::generateConfig(const Device& d, const ConfigState& state) {
    std::ostringstream out;

    // Header
    out << "{ config, pkgs, }: {\n";

    // ------------------- Global System Settings -------------------
    out << "  networking.hostName = \"" << state.getHostname() << "\";\n";
    out << "  services.openssh.enable = " << (state.isOpenSSHEnabled() ? "true" : "false") << ";\n";
    out << "  networking.firewall.enable = " << (state.isFirewallEnabled() ? "true" : "false") << ";\n";

    if (!state.getTimezone().empty())
        out << "  time.timeZone = \"" << state.getTimezone() << "\";\n";

    if (!state.getLocale().empty())
        out << "  i18n.defaultLocale = \"" << state.getLocale() << "\";\n";

    // ------------------- Users -------------------
    if (!state.getUsers().empty()) {
        out << "  users.users = {\n";
        for (const auto& [uname, info] : state.getUsersMap()) {
            out << "    " << uname << " = {\n";
            out << "      isNormalUser = true;\n";
            out << "      description = \"" << info.description << "\";\n";
            out << "      shell = \"" << info.shell << "\";\n";
            out << "      home = \"" << info.home << "\";\n";

            // Build groups
            std::string groups = info.groups;
            if (info.isSuperuser) {
                if (!groups.empty()) groups += " ";
                groups += "wheel";
            }
            if (!groups.empty())
                out << "      extraGroups = [ " << groups << " ];\n";

            out << "    };\n";
        }
        out << "  };\n";
    }


    // ------------------- Packages -------------------
    const auto& packages = state.getPackages();
    if (!packages.empty()) {
        out << "  environment.systemPackages = with pkgs; [";
        for (const auto& [pkgName, _] : packages) {
            out << " " << pkgName;
        }
        out << " ];\n";
    }

    // ------------------- Custom Block -------------------
    if (!state.getCustomBlock().empty()) {
        out << "  " << state.getCustomBlock() << "\n";
    }

    out << "}\n";
    return out.str();
}
