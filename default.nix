{ pkgs ? import <nixpkgs> {} }:

pkgs.stdenv.mkDerivation rec {
  pname = "nixman";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [
    pkgs.cmake
  ];

  buildInputs = [
    pkgs.ncurses
    pkgs.nlohmann_json
    pkgs.gcc   # Ensure compiler available
  ];

  # Ensure a fresh build directory each time
  buildPhase = ''
    rm -rf build
    mkdir build
    cmake -S $src -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${pkgs.ncurses}/${pkgs.nlohmann_json}
    cmake --build build
  '';

  installPhase = ''
    mkdir -p $out/bin
    cp build/Nixman $out/bin/
  '';

  meta = with pkgs.lib; {
    description = "TUI-based NixOS configuration and deployment tool";
    license = licenses.gpl3;
    platforms = platforms.linux;
  };
}
