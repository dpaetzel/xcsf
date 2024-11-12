{

  inputs = {
    # For test devShell only.
    nixpkgs.url =
      #   # "github:dpaetzel/nixpkgs/dpaetzel/nixos-config";
      "github:dpaetzel/nixpkgs/update-clipmenu";
    # For test devShell only.
    overlays.url = "github:dpaetzel/overlays/master";

    dSFMTSource.url = "github:MersenneTwister-Lab/dSFMT";
    dSFMTSource.flake = false;
  };

  outputs =
    {
      self,
      nixpkgs, # for devShell only
      overlays, # for devShell only
      dSFMTSource,
    }:
    let
      xcsfOverlay = final: prev: {
        pythonPackagesExtensions = prev.pythonPackagesExtensions ++ [
          (
            python-final: python-prev:

            # XCSF's build process expects the dSFMT library source to be available.
            # Therefore, we make it available.
            let
              dSFMT = prev.pkgs.stdenv.mkDerivation {
                name = "dSFMT";
                version = "dev";

                src = dSFMTSource;
                dontConfigure = true;
                dontBuild = true;
                installPhase = ''
                  mkdir "$out"
                  cp -r * "$out"
                '';
              };
            in
            {
              xcsf = python-prev.toPythonModule (
                prev.pkgs.stdenv.mkDerivation rec {
                  enablePython = true;
                  # pythonPackages = python-prev.python;
                  # I'm right here. I have no clue what I should put into this
                  # field. python-{prev,final} don't work. Others put self but
                  #   that seems wrong (b/c self=prev and therefore should be
                  #   python-prev).
                  # pythonPackages = python-final;

                  pname = "xcsf";
                  version = "1.3.1";

                  src = self;

                  patchPhase = ''
                    sed -i 's|add_subdirectory(lib/pybind11)|find_package(pybind11 CONFIG)|' \
                        CMakeLists.txt
                    sed -i 's|''${CMAKE_SOURCE_DIR}/lib/dSFMT|${dSFMT}|' \
                        xcsf/CMakeLists.txt
                    sed -i 's|''${CMAKE_SOURCE_DIR}/lib/cJSON|${prev.pkgs.cjson.src}|' \
                        xcsf/CMakeLists.txt
                    sed -i 's|../lib/cJSON|${prev.pkgs.cjson.src}|' xcsf/utils.h
                    sed -i 's|../lib/dSFMT|${dSFMT}|' xcsf/utils.h
                    sed -i 's|../lib/doctest|${prev.pkgs.doctest.src}|' test/*.cpp
                  '';

                  nativeBuildInputs = [
                    prev.pkgs.extra-cmake-modules
                    python-prev.pybind11
                  ];

                  propagatedBuildInputs = [
                    python-prev.numpy
                  ];

                  buildInputs = [
                    prev.doxygen
                    prev.graphviz
                    # We generally have pkgs.python312.pkgs.python ==
                    # pkgs.python312 and thus `python-prev.python` should refer
                    # to the Python version that we're currently overwriting
                    # since `python-prev == pkgs.python312.pkgs`.
                    python-prev.python
                  ];
                  # TODO Add openmp?

                  cmakeFlags = [
                    "-DCMAKE_BUILD_TYPE=RELEASE"

                    "-DXCSF_PYLIB=ON"
                    "-DENABLE_TESTS=ON"
                    "-DPARALLEL=ON"
                  ];

                  installPhase = ''
                    mkdir -p $out/${python-prev.python.sitePackages}
                    cp xcsf/xcsf.*.so $out/${python-prev.python.sitePackages}
                  '';

                  meta = with prev.lib; {
                    description = "Implementation of the XCSF learning classifier system";
                    longDescription = ''
                      Preen's Python bindings for his implementation of the XCSF learning
                      classifier system in C.
                    '';
                    homepage = "https://github.com/rpreen/xcsf";
                    license = licenses.gpl3;
                    maintainers = [ maintainers.dpaetzel ];
                    platforms = platforms.all;
                  };
                }
              );
            }

          )
        ];
      };

    in
    {
      overlays.xcsf = xcsfOverlay;

      # You can test this by doing `nix develop path:path/to/this/repo`. Make
      # sure to change directory beforehand so Python doesn't attempt to import
      # from this repository's `xcsf` folder.
      devShell.x86_64-linux =
        let
          pkgs = import nixpkgs {
            system = "x86_64-linux";
            overlays = [
              overlays.overlays.mydefaults
              xcsfOverlay
            ];
          };
          # For `nix repl` after `:lf .`.
          # pkgs = import inputs.nixpkgs {
          #   system = "x86_64-linux";
          #   overlays = [
          #     # overlays.overlays.mydefaults
          #     outputs.overlays.xcsf
          #   ];
          # }
        in

        pkgs.mkShell {
          buildInputs = [
            (pkgs.mypython.withPackages (ps: [
              ps.xcsf

              # Required for running the examples.
              ps.gymnasium
              ps.matplotlib
              ps.tkinter
              ps.tqdm
            ]))
          ];

          shellHook = ''
            cwd=$(pwd)
            echo Entering temp dir …
            cd $(mktemp --directory)
            echo Checking whether import works …
            python -c 'import xcsf'\
                 && echo '✅ Importing works!' \
                 || true
            echo Switching back to original directory …
            cd "$cwd"
          '';
        };
    };
}
