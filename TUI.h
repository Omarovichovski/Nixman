#ifndef TUI_H
#define TUI_H

#include <vector>
#include <string>
#include "ConfigState.h"
#include "Device.h"
#include "ConfigGenerator.h"
#include "NixQuery.h"
#include "NixPackage.h"
#include "Inventory.h"
#include "Deployer.h"

class TUI {
private:
    std::vector<NixPackage> selectedPackages;    // Packages selected by user
    ConfigState configState;                     // Global system settings
    NixQuery nixQuery;                           // Package search engine
    Inventory inventory;                         // Device inventory
    

public:
    void run();                                  // Main loop

private:
    // Main menu
    void showMainMenu();

    // Users
    void viewUsers();
    void editUser(const std::string& username);

    // Package viewing & editing
    void viewSelectedPackages();                 // View/remove/edit packages
    void editPackageOptions(NixPackage& pkg);   // Edit options of a single package

    // Search & add packages
    void searchAndAddPackages();                 // Search nixpkgs and add packages
    
    // Services
    void editServices();

    // Deployment
    void deployMenu();

    // Global system settings
    void editGlobalSettings();                   // Hostname, OpenSSH, custom blocks

    // Generate configuration
    void generateConfig(Device& device);        // Produce NixOS config
};

#endif
