{
  description = "NEON-WORLD-OS (C++) - reproducible build via Nix flake";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f (import nixpkgs { inherit system; }));
    in {
      packages = forAllSystems (pkgs: {
        default = pkgs.stdenv.mkDerivation {
          pname = "neon-world-os";
          version = "280.0.0";
          src = ./.;
          nativeBuildInputs = [ pkgs.cmake pkgs.ninja pkgs.pkg-config ];
          cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];
          configurePhase = "cmake -S . -B build -G Ninja $cmakeFlags";
          buildPhase = "cmake --build build -j";
          installPhase = "cmake --install build --prefix $out";
        };
      });
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          buildInputs = [ pkgs.cmake pkgs.ninja pkgs.clang pkgs.gdb pkgs.pkg-config ];
        };
      });
    };
}
