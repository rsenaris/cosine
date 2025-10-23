{ self
, nixpkgs # <---- This `nixpkgs` has systems removed e.g. legacyPackages.zlib
, nain4
, ...
}: let
  inherit (nixpkgs.legacyPackages) pkgs;
  inherit (import ./helpers.nix {inherit pkgs;}) shell-shared;
  inherit (nain4.deps) args-from-cli make-app;
  in rec {

    packages.default = self.packages.cosine;

    cosine-deps = with pkgs; [hdf5-cpp hdf5-cpp.dev highfive];

    # Executed by `nix run <URL of this flake>#cosine -- <args?>`
    # TODO: switch to clang environment
    packages.cosine = pkgs.stdenv.mkDerivation {
      pname = "cosine";
      version = "0.0.0";
      src = "${self}/src";
      postInstall = "${pkgs.coreutils}/bin/cp -r ${self}/macs $out";
      nativeBuildInputs = [];
      buildInputs = [ nain4.packages.nain4 ] ++ cosine-deps;
    };

    # Executed by `nix run <URL of this flake> -- <args?>`
    apps.default = self.apps.cosine;

    # Executed by `nix run <URL of this flake>#cosine`
    apps.cosine = make-app {
      executable = "cosine";
      args = "--macro-path ${self}/macs ${args-from-cli}";
      package = self.packages.default;
    };

    # Used by `direnv` when entering this directory (also by `nix develop <URL to this flake>`)
    devShell = self.devShells.clang;

    # Activated by `nix develop <URL to this flake>#clang`
    devShells.clang = pkgs.mkShell.override { stdenv = nain4.packages.clang_current.stdenv; } (shell-shared // {
      name = "cosine-clang-devenv";
      packages = nain4.deps.dev-shell-packages ++ [ nain4.packages.clang_current ] ++ cosine-deps;
    });

    # Activated by `nix develop <URL to this flake>#gcc`
    devShells.gcc = pkgs.mkShell (shell-shared // {
      name = "cosine-gcc-devenv";
      packages = nain4.deps.dev-shell-packages ++ cosine-deps;
    });

    # 1. `nix build` .#singularity
    # 2. `scp result <me>@lxplus7.cern.ch:hello.img`
    # 3. [on lxplus] `singularity run hello.img`
    packages.singularity = pkgs.singularity-tools.buildImage {
      name = "cosine";
      contents = [ self.apps.cosine.program ];
      runScript = "${self.apps.cosine.program} $@";
      diskSize = 10240;
      memSize = 5120;
    };

  }
