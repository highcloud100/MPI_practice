{
  description = "MPI Study Environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShells.default = pkgs.mkShell {
          # 컴파일 타임 도구 (Build tools)
          nativeBuildInputs = with pkgs; [
            cmake
            gnumake
            pkg-config
          ];

          # 런타임 라이브러리 및 MPI 구현체 (Runtime dependencies)
          buildInputs = with pkgs; [
            openmpi       # 또는 mpich 사용 시: pkgs.mpich
            gdb           # 디버거
            valgrind      # 메모리 누수 탐지
          ];

          # 환경 변수 설정
          shellHook = ''
            echo "------------------------------------------------"
            echo "OpenMPI Version: $(mpirun --version | head -n 1)"
            echo "Compiler: $(mpicc --version | head -n 1)"
            echo "------------------------------------------------"
            
            # OpenMPI가 로컬 실행 시 불필요한 SSH 경고를 띄우지 않도록 설정
            export OMPI_MCA_plm_rsh_agent=false
          '';
        };
      }
    );
}