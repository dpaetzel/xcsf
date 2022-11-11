{
  description = "The Python XCSF library";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }: {

    defaultPackage.x86_64-linux =
      with import nixpkgs { system = "x86_64-linux"; };
      let python = python310;
      in python.pkgs.toPythonModule (stdenv.mkDerivation rec {
        # Note that as of 2021-12-15 this does not build from a checked-out git
        # repository due to the submodules. See
        # https://github.com/NixOS/nix/issues/5312 .
        pname = "xcsf";
        version = "1.1.4";

        src = self;

        nativeBuildInputs = [ cmake ];

        buildInputs = [ python ];
        # TODO Add openmp?

        propagatedBuildInputs = with python.pkgs; [
          doxygen
          graphviz
          numpy
          tqdm
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=RELEASE"

          "-DXCSF_PYLIB=ON"
          "-DENABLE_TESTS=ON"
          "-DPARALLEL=ON"
        ];

        # NIX_LDFLAGS = [ "-lstdc++" ];

        installPhase = ''
          mkdir -p $out/${python.sitePackages}
          cp -r xcsf $out/${python.sitePackages}
          rm -r $out/${python.sitePackages}/xcsf/{cmake_install.cmake,CMakeFiles,CTestTestfile.cmake,Makefile}
          rm $out/${python.sitePackages}/xcsf/main
        '';

        # runtimeDependencies = [ stdenv.cc.cc ];

        meta = with lib; {
          description = "Implementation of the XCSF learning classifier system";
          longDescription = ''
            Preen's Python bindings for his implementation of the XCSF learning
            classifier system in C.
          '';
          homepage = "https://github.com/rpreen/xcsf";
          license = licenses.gpl3;
          platforms = platforms.all;
        };
      });
  };
}
