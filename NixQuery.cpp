#include "NixQuery.h"
#include "NixPackage.h"
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>
#include <nlohmann/json.hpp>

// Helper to strip ANSI escape codes
static std::string stripAnsi(const std::string& input) {
    static const std::regex ansi_regex("\x1B\\[[0-9;]*[mK]");
    return std::regex_replace(input, ansi_regex, "");
}

// ==================== QUERY MODULE OPTIONS ====================
std::vector<NixOption> NixQuery::queryModuleOptions(const std::string& prefix) {
    std::vector<NixOption> results;
    std::string command = "nix eval --json '(import <nixpkgs/nixos> {}).options' 2>/dev/null";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return results;

    std::string output;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
    pclose(pipe);

    if (output.empty()) return results;

    try {
        auto json = nlohmann::json::parse(output);
        for (auto it = json.begin(); it != json.end(); ++it) {
            const std::string& fullName = it.key();
            if (fullName.rfind(prefix, 0) != 0) continue;

            const auto& opt = it.value();
            NixOption option;
            option.name = fullName;
            if (opt.contains("type")) option.type = opt["type"].dump();
            if (opt.contains("description") && opt["description"].is_string())
                option.description = opt["description"];
            if (opt.contains("default")) option.defaultValue = opt["default"].dump();
            results.push_back(option);
        }
    } catch (...) {}

    return results;
}

// ==================== SEARCH PACKAGES ====================
std::vector<NixPackage> NixQuery::search(const std::string& term) {
    std::vector<NixPackage> results;
    std::string command = "nix search nixpkgs " + term + " 2>/dev/null";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return results;

    char buffer[512];
    NixPackage currentPkg;
    bool inPackage = false;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        line = stripAnsi(line);
        if (!line.empty() && line.back() == '\n') line.pop_back();
        line.erase(line.begin(),
                   std::find_if(line.begin(), line.end(), [](unsigned char ch){ return !std::isspace(ch); }));
        if (line.empty()) continue;

        if (line.size() > 2 && line[0] == '*' && line[1] == ' ') {
            if (inPackage) results.push_back(currentPkg);

            std::string pkgLine = line.substr(2);
            size_t endPos = pkgLine.find_first_of(" (");
            std::string pkgName = pkgLine.substr(0, endPos);

            currentPkg = NixPackage();
            currentPkg.name = pkgName;
            currentPkg.description.clear();
            currentPkg.selected = false;
            currentPkg.options.clear();
            currentPkg.moduleName.clear();  // ensure moduleName empty
            inPackage = true;
        } else if (inPackage) {
            if (!currentPkg.description.empty()) currentPkg.description += " ";
            currentPkg.description += line;
        }
    }

    if (inPackage) results.push_back(currentPkg);
    pclose(pipe);
    return results;
}

// ==================== DETECT MODULE PREFIX ====================
std::string NixQuery::detectModulePrefix(const std::string& name) {
    std::vector<std::string> prefixes = {
        "programs.",
        "services.",
        "hardware.",
        "virtualisation.",
        "users.users."
    };

    for (const auto& pre : prefixes) {
        std::string full = pre + name;
        std::string cmd = "nix eval --json '(import <nixpkgs/nixos> {}).options." + full + "' 2>/dev/null";

        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) continue;

        char buffer[4096];
        std::string output;
        while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
        pclose(pipe);

        if (!output.empty()) return full; // module exists
    }

    return ""; // no module found
}

// ==================== CHECK IF MODULE EXISTS ====================
bool NixQuery::moduleExists(const std::string& modulePrefix) {
    if (modulePrefix.empty()) return false;

    std::string cmd = "nix eval --json '(import <nixpkgs/nixos> {}).options." + modulePrefix + "' 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return false;

    char buffer[4096];
    std::string output;
    while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
    pclose(pipe);

    return !output.empty();
}

// ==================== GET PACKAGE OPTIONS ====================
std::vector<NixOption> NixQuery::getPackageOptions(const std::string& name, bool alreadyModule) {
    std::vector<NixOption> options;

    // Detect module only if not already passed
    std::string prefix = alreadyModule ? name : detectModulePrefix(name);
    if (prefix.empty()) return options;

    // Query NixOS options directly from the module path
    std::string cmd = "nix eval --json '(import <nixpkgs/nixos> {}).options." + prefix + "' 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return options;

    std::string output;
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe)) output += buffer;
    pclose(pipe);

    if (output.empty()) return options;

    try {
        auto j = nlohmann::json::parse(output);
        for (auto it = j.begin(); it != j.end(); ++it) {
            NixOption opt;
            opt.name = it.key();
            const auto& v = it.value();
            if (v.contains("type")) opt.type = v["type"].dump();
            if (v.contains("description") && v["description"].is_string())
                opt.description = v["description"];
            if (v.contains("default")) opt.defaultValue = v["default"].dump();
            options.push_back(opt);
        }
    } catch (...) {}

    return options;
}
