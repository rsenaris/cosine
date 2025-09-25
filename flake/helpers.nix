{pkgs} :
{
  shell-shared = {
    G4_DIR = "${pkgs.geant4}";

    shellHook = ''
      export cosine_lib_PATH=$PWD/install/cosine/lib
      export LD_LIBRARY_PATH=$cosine_lib_PATH:$LD_LIBRARY_PATH;
      export PKG_CONFIG_PATH=$cosine_lib_PATH/pkgconfig:$PKG_CONFIG_PATH;
    '';
  };
}
