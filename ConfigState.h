#ifndef CONFIGSTATE_H
#define CONFIGSTATE_H

#include <string>
#include <map>

class ConfigState {
public:
    struct UserInfo {
        std::string description;
        std::string shell;
        std::string home;
        std::string groups;      // additional groups
        bool isSuperuser = false; // new field for superuser
    };

private:
    // Packages
    std::map<std::string, std::map<std::string, std::string>> packages;

    // Global settings
    std::string customBlock;
    std::string hostname;
    bool enableOpenSSH = true;
    std::string timezone;
    std::string locale;
    bool firewallEnabled = true;

    // Users
    std::map<std::string, UserInfo> usersMap;

public:
    // ==================== Packages ====================
    void addPackage(const std::string& pkgName, const std::map<std::string, std::string>& options = {});
    void removePackage(const std::string& pkgName);
    const std::map<std::string, std::map<std::string, std::string>>& getPackages() const;
    const std::map<std::string, std::string>& getPackageOptions(const std::string& pkgName) const;

    // ==================== Global Settings ====================
    void setCustomBlock(const std::string& block) { customBlock = block; }
    std::string getCustomBlock() const { return customBlock; }

    void setHostname(const std::string& host) { hostname = host; }
    std::string getHostname() const { return hostname; }

    void setOpenSSH(bool enabled) { enableOpenSSH = enabled; }
    bool isOpenSSHEnabled() const { return enableOpenSSH; }

    void setTimezone(const std::string& tz) { timezone = tz; }
    std::string getTimezone() const { return timezone; }

    void setLocale(const std::string& lc) { locale = lc; }
    std::string getLocale() const { return locale; }

    void setFirewallEnabled(bool f) { firewallEnabled = f; }
    bool isFirewallEnabled() const { return firewallEnabled; }

    // ==================== Users ====================
    void setUser(const std::string& username, const UserInfo& info) { usersMap[username] = info; }
    void removeUser(const std::string& username) { usersMap.erase(username); }
    const std::map<std::string, UserInfo>& getUsersMap() const { return usersMap; }

    // Optional backward-compatible: comma-separated user names
    std::string getUsers() const;
};

#endif
