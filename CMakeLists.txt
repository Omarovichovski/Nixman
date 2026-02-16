cmake_minimum_required(VERSION 3.10)
project(Nixman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow CMake to search additional paths (useful for Nix builds)
list(APPEND CMAKE_PREFIX_PATH "$ENV{CMAKE_PREFIX_PATH}")

# Find ncurses
find_package(Curses REQUIRED)

# Find nlohmann_json because i ABSOLUTELY do NOT want to manually parse json
find_package(nlohmann_json REQUIRED)

# Executable
add_executable(Nixman
    main.cpp
    Device.cpp
    Inventory.cpp
    ConfigGenerator.cpp
    ConfigState.cpp
    NixQuery.cpp
    SSHManager.cpp
    FileUtils.cpp
    Deployer.cpp
    TUI.cpp
)

# Link libraries
target_link_libraries(Nixman
    ${CURSES_LIBRARIES}
    nlohmann_json::nlohmann_json
    pthread
)

# Include directories
target_include_directories(Nixman PRIVATE ${CURSES_INCLUDE_DIR})
