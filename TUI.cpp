#include "TUI.h"
#include <ncurses.h>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <regex>

// ==================== Main Loop ====================
void TUI::run() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (true) {
        showMainMenu();
    }

    endwin();
}

// ==================== Main Menu ====================
void TUI::showMainMenu() {
    clear();
    mvprintw(0, 0, "=== NixOS Config Generator - TUI ===");
    mvprintw(2, 0, "1) View/Modify Selected Packages");
    mvprintw(3, 0, "2) Search & Add Packages");
    mvprintw(4, 0, "3) Global System Settings");
    mvprintw(5, 0, "4) Generate Configuration");
    mvprintw(6, 0, "5) Deploy To All Devices");
    mvprintw(7, 0, "6) Exit");

    int choice = getch();

    switch (choice) {
        case '1': viewSelectedPackages(); break;
        case '2': searchAndAddPackages(); break;
        case '3': editGlobalSettings(); break;
        case '4': {
            Device dummyDevice;
            generateConfig(dummyDevice);
            break;
        }
        case '5': break; // TODO: Deploy
        case '6': endwin(); std::exit(0);
    }
}


// ==================== View/Modify Selected Packages ====================
void TUI::viewSelectedPackages() {
    if (selectedPackages.empty()) {
        clear();
        mvprintw(0, 0, "No packages selected yet. Press any key to return...");
        getch();
        return;
    }

    int highlight = 0;

    while (true) {
        clear();
        mvprintw(0, 0, "Selected Packages (ENTER to edit options, SPACE to remove, UP/DOWN to navigate, ESC to return)");

        int displayStart = std::max(0, highlight - (LINES - 3));
        int displayEnd = std::min((int)selectedPackages.size(), displayStart + LINES - 3);

        for (int i = displayStart; i < displayEnd; ++i) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(i - displayStart + 2, 0, "%s", selectedPackages[i].name.c_str());
            if (i == highlight) attroff(A_REVERSE);
        }

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (highlight > 0) highlight--; break;
            case KEY_DOWN: if (highlight < (int)selectedPackages.size() - 1) highlight++; break;
            case ' ':
                selectedPackages.erase(selectedPackages.begin() + highlight);
                if (highlight >= (int)selectedPackages.size())
                    highlight = selectedPackages.size() - 1;
                break;
            case 10: // ENTER -> edit options
                if (!selectedPackages.empty())
                    editPackageOptions(selectedPackages[highlight]);
                break;
            case 27: // ESC
                return;
        }

        if (selectedPackages.empty()) {
            clear();
            mvprintw(0, 0, "All packages removed. Press any key to return...");
            getch();
            return;
        }
    }
}

// ==================== Edit Package Options ====================
void TUI::editPackageOptions(NixPackage& pkg) {
    // Ensure at least one option exists
    if (pkg.options.empty()) {
        pkg.options["exampleOption"] = "default";
    }

    std::vector<std::string> keys;
    for (auto& [key, _] : pkg.options) keys.push_back(key);

    int highlight = 0;
    while (true) {
        clear();
        mvprintw(0, 0, "Editing options for %s (ENTER to edit, 'r' reset, 'a' add, 'd' delete, ESC to return)", pkg.name.c_str());

        for (size_t i = 0; i < keys.size(); ++i) {
            if ((int)i == highlight) attron(A_REVERSE);
            mvprintw(i + 2, 0, "%s = %s", keys[i].c_str(), pkg.options[keys[i]].c_str());
            if ((int)i == highlight) attroff(A_REVERSE);
        }

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (highlight > 0) highlight--; break;
            case KEY_DOWN: if (highlight < (int)keys.size() - 1) highlight++; break;
            case 10: { // ENTER -> edit value
                echo();
                char newVal[512];
                mvprintw(LINES - 2, 0, "Enter new value: ");
                getnstr(newVal, 511);
                noecho();
                pkg.options[keys[highlight]] = newVal;
                break;
            }
            case 'r': // reset value
                pkg.options[keys[highlight]] = "default";
                break;
            case 'a': { // add new option
                echo();
                char newKey[128], newVal[256];
                mvprintw(LINES - 2, 0, "Enter new option key: ");
                getnstr(newKey, 127);
                mvprintw(LINES - 1, 0, "Enter value: ");
                getnstr(newVal, 255);
                noecho();
                std::string keyStr(newKey);
                if (!keyStr.empty()) {
                    pkg.options[keyStr] = newVal;
                    keys.push_back(keyStr);
                }
                break;
            }
            case 'd': // delete option
                pkg.options.erase(keys[highlight]);
                keys.erase(keys.begin() + highlight);
                if (highlight >= (int)keys.size()) highlight = keys.size() - 1;
                break;
            case 27: // ESC -> exit
                return;
        }
    }
}


// ==================== Search & Add Packages ====================
void TUI::searchAndAddPackages() {
    echo();
    char query[128];
    mvprintw(7, 0, "Enter search term: ");
    getnstr(query, 127);
    noecho();

    auto rawResults = nixQuery.search(query);
    if (rawResults.empty()) {
        clear();
        mvprintw(0, 0, "No packages found for '%s'. Press any key to return...", query);
        getch();
        return;
    }

    std::vector<NixPackage> results = rawResults;
    int highlight = 0, pageStart = 0;
    const int availableLines = LINES - 3;
    const int wrapWidth = COLS - 4;

    while (true) {
        clear();
        int lineNum = 1;
        int pageEnd = pageStart;

        while (pageEnd < (int)results.size() &&
               lineNum + 1 + (results[pageEnd].description.size() + wrapWidth - 1)/wrapWidth <= availableLines) {
            lineNum += 1 + (results[pageEnd].description.size() + wrapWidth - 1)/wrapWidth;
            pageEnd++;
        }

        mvprintw(0, 0, "Search results for '%s' (Package %d/%d)", query, highlight + 1, (int)results.size());

        lineNum = 1;
        for (int i = pageStart; i < pageEnd; ++i) {
            auto& pkg = results[i];
            if (i == highlight) attron(A_REVERSE);
            attron(A_BOLD);
            mvprintw(lineNum++, 0, "[%c] %s", pkg.selected ? 'x' : ' ', pkg.name.c_str());
            attroff(A_BOLD);

            std::string desc = pkg.description;
            size_t pos = 0;
            while (pos < desc.size() && lineNum < LINES - 1) {
                mvprintw(lineNum++, 4, "%s", desc.substr(pos, wrapWidth).c_str());
                pos += wrapWidth;
            }
            if (i == highlight) attroff(A_REVERSE);
        }

        mvprintw(LINES - 1, 0, "UP/DOWN scroll, SPACE select, ENTER confirm, n/p next/prev page");

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (highlight > 0) highlight--; if (highlight < pageStart) pageStart = highlight; break;
            case KEY_DOWN: if (highlight < (int)results.size() - 1) highlight++; if (highlight >= pageEnd) pageStart++; break;
            case ' ': results[highlight].selected = !results[highlight].selected; break;
            case 'n': if (pageEnd < (int)results.size()) pageStart = pageEnd; break;
            case 'p': if (pageStart > 0) { int tmp = pageStart - 1, lineSum = 0; while (tmp >= 0 && lineSum + 1 + (results[tmp].description.size() + wrapWidth - 1)/wrapWidth <= availableLines) { lineSum += 1 + (results[tmp].description.size() + wrapWidth - 1)/wrapWidth; tmp--; } pageStart = tmp + 1; } break;
            case 10: // ENTER -> confirm selection
                for (auto& pkg : results) {
                    if (!pkg.selected) continue;

                    // Add package to ConfigState using plain name
                    configState.addPackage(pkg.name, {});  // empty options, ignored now

                    // Keep for UI view
                    selectedPackages.push_back(pkg);
                }
                return;
        }
    }
}



// ==================== Global System Settings ====================
void TUI::editGlobalSettings() {
    int highlight = 0;
    const int optionsCount = 7; // Hostname, OpenSSH, Timezone, Locale, Users, Firewall, Custom
    bool done = false;

    while (!done) {
        clear();
        mvprintw(0, 0, "=== Global System Settings === (ENTER to edit, ESC to return)");

        mvprintw(2, 0, "%s Hostname: %s", (highlight==0?"->":"  "), configState.getHostname().c_str());
        mvprintw(3, 0, "%s OpenSSH: %s", (highlight==1?"->":"  "), configState.isOpenSSHEnabled()?"Enabled":"Disabled");
        mvprintw(4, 0, "%s Timezone: %s", (highlight==2?"->":"  "), configState.getTimezone().c_str());
        mvprintw(5, 0, "%s Locale: %s", (highlight==3?"->":"  "), configState.getLocale().c_str());
        mvprintw(6, 0, "%s Users: %d user(s)", (highlight==4?"->":"  "), (int)configState.getUsersMap().size());
        mvprintw(7, 0, "%s Firewall: %s", (highlight==5?"->":"  "), configState.isFirewallEnabled()?"Enabled":"Disabled");
        mvprintw(8, 0, "%s Custom block: %s", (highlight==6?"->":"  "), configState.getCustomBlock().empty()?"<empty>":configState.getCustomBlock().c_str());

        int ch = getch();
        switch (ch) {
            case KEY_UP: if (highlight>0) highlight--; break;
            case KEY_DOWN: if (highlight<optionsCount-1) highlight++; break;

            case 10: // ENTER
                switch (highlight) {
                    case 0: { // Hostname
                        echo();
                        char buf[128];
                        mvprintw(LINES-2, 0, "Enter hostname: ");
                        getnstr(buf, 127);
                        noecho();
                        if (strlen(buf) > 0) configState.setHostname(buf);
                        break;
                    }
                    case 1: { // OpenSSH
                        mvprintw(LINES-2, 0, "Enable OpenSSH? (y/n): ");
                        int key = getch();
                        configState.setOpenSSH(key=='y'||key=='Y');
                        break;
                    }
                    case 2: { // Timezone
                        echo();
                        char buf[128];
                        mvprintw(LINES-2, 0, "Enter timezone (e.g., Europe/Skopje): ");
                        getnstr(buf, 127);
                        noecho();
                        if (strlen(buf)>0) configState.setTimezone(buf);
                        break;
                    }
                    case 3: { // Locale
                        echo();
                        char buf[64];
                        mvprintw(LINES-2, 0, "Enter locale (e.g., en_US.UTF-8): ");
                        getnstr(buf, 63);
                        noecho();
                        if (strlen(buf)>0) configState.setLocale(buf);
                        break;
                    }
                    case 4: { // Users
                        std::vector<std::string> usernames;
                        for (const auto& [u, _] : configState.getUsersMap())
                            usernames.push_back(u);
                        int userHighlight = 0;
                        bool editingUsers = true;

                        while (editingUsers) {
                            clear();
                            mvprintw(0, 0, "=== Users === (ENTER edit, a add, d delete, ESC return)");
                            int line = 2;
                            for (size_t i = 0; i < usernames.size(); ++i) {
                                mvprintw(line++, 0, "%s %s", (int)i==userHighlight?"->":"  ", usernames[i].c_str());
                            }

                            int c = getch();
                            switch(c) {
                                case KEY_UP: if(userHighlight>0) userHighlight--; break;
                                case KEY_DOWN: if(userHighlight<(int)usernames.size()-1) userHighlight++; break;
                                case 10: { // edit selected user
                                    const std::string& uname = usernames[userHighlight];
                                    auto user = configState.getUsersMap().at(uname); // copy
                                    echo();
                                    char buf[128];
                                    // Description
                                    mvprintw(LINES-5,0,"Description (%s): ", user.description.c_str());
                                    getnstr(buf,127); if(strlen(buf)>0) user.description=buf;
                                    // Superuser status
                                    mvprintw(LINES-4,0,"Superuser? (y/n) [%s]: ", user.isSuperuser?"y":"n");
                                    int key = getch();
                                    if (key=='y'||key=='Y') user.isSuperuser = true;
                                    else if (key=='n'||key=='N') user.isSuperuser = false;

                                    // Shell
                                    mvprintw(LINES-3,0,"Shell (%s): ", user.shell.c_str());
                                    getnstr(buf,127); if(strlen(buf)>0) user.shell=buf;
                                    // Home
                                    mvprintw(LINES-2,0,"Home (%s): ", user.home.c_str());
                                    getnstr(buf,127); if(strlen(buf)>0) user.home=buf;
                                    // Groups
                                    mvprintw(LINES-1,0,"Groups (%s): ", user.groups.c_str());
                                    getnstr(buf,127); if(strlen(buf)>0) user.groups=buf;
                                    noecho();
                                    configState.setUser(uname,user);
                                    break;
                                }
                                case 'a': { // add user
                                    echo();
                                    char buf[128];
                                    mvprintw(LINES-2,0,"Enter new username: ");
                                    getnstr(buf,127);
                                    std::string uname(buf);
                                    noecho();
                                    if(!uname.empty()) {
                                        ConfigState::UserInfo info;
                                        info.description = uname;
                                        info.shell = "/bin/bash";
                                        info.home = "/home/"+uname;
                                        info.groups = "users";
                                        configState.setUser(uname,info);
                                        usernames.push_back(uname);
                                        userHighlight = usernames.size()-1;
                                    }
                                    break;
                                }
                                case 'd': { // delete user
                                    if(!usernames.empty()) {
                                        std::string uname = usernames[userHighlight];
                                        configState.removeUser(uname);
                                        usernames.erase(usernames.begin()+userHighlight);
                                        if(userHighlight >= (int)usernames.size()) userHighlight = usernames.size()-1;
                                    }
                                    break;
                                }
                                case 27: editingUsers=false; break;
                            }
                        }
                        break;
                    }
                    case 5: { // Firewall
                        mvprintw(LINES-2, 0, "Enable firewall? (y/n): ");
                        int key = getch();
                        configState.setFirewallEnabled(key=='y'||key=='Y'); // use your version
                        break;
                    }
                    case 6: { // Custom block
                        echo();
                        char buf[512];
                        mvprintw(LINES-2, 0, "Enter custom NixOS config block: ");
                        getnstr(buf, 511);
                        noecho();
                        configState.setCustomBlock(buf);
                        break;
                    }
                }
                break;

            case 27: // ESC
                done = true;
                break;
        }
    }
}
// ==================== Generate Configuration ====================
void TUI::generateConfig(Device& device) {
    // Clear previous packages from ConfigState
    for (auto& pkg : selectedPackages)
        configState.removePackage(pkg.name);

    // Re-add all selected packages using plain names
    for (auto& pkg : selectedPackages)
        configState.addPackage(pkg.name, {});

    ConfigGenerator generator;
    std::string config = generator.generateConfig(device, configState);

    clear();
    mvprintw(0, 0, "Generated configuration:\n%s", config.c_str());
    mvprintw(LINES - 1, 0, "Press any key to return...");
    getch();
}