name: Linux builds (basic)

on: [push, pull_request]

jobs:
  build:
    name: ${{matrix.compiler.cxx}}, C++${{matrix.std}}, ${{matrix.build_type}}
    runs-on: ubuntu-22.04
    strategy:
      fail-fast: false
      matrix:
        compiler:
          - {cxx: g++-10, other_pkgs: g++-10}
          - {cxx: g++-11, other_pkgs: g++-11}
          - {cxx: clang++-11, other_pkgs: clang-11}
          - {cxx: clang++-12, other_pkgs: clang-12}
          - {cxx: clang++-13, other_pkgs: clang-13}
          - {cxx: clang++-14, other_pkgs: clang-14}
        build_type: [Debug, Release]
        std: [11, 14, 17, 20]
        #std: [11, 14, 17, 20]
        #exclude:
        #  - compiler.cxx: "g++-{5,6,7,8,9,10}"
        #    std: 17
        #  - compiler.cxx: "g++-{5,6,7,8,9,10}"
        #    std: 20
        #  - compiler.cxx: "clang++-{6.0,7,8,9,10}"
        #    std: 17
        #  - compiler.cxx: "clang++-{6.0,7,8,9,10}"
        #    std: 20

    steps:
    - uses: actions/checkout@v4

    - name: Add repositories for older GCC
      run: |
        sudo apt-add-repository 'deb http://azure.archive.ubuntu.com/ubuntu/ bionic main'
        sudo apt-add-repository 'deb http://azure.archive.ubuntu.com/ubuntu/ bionic universe'
      if: ${{ matrix.compiler.cxx == 'g++-5' || matrix.compiler.cxx == 'g++-6' }}

    - name: Prepare environment
      run: |
        sudo apt-get update
        sudo apt-get install -y ninja-build ${{matrix.compiler.other_pkgs}}

    - name: Configure CMake
      env:
        CXX: ${{matrix.compiler.cxx}}
      run: |
        cmake -B build \
              -DCMAKE_BUILD_TYPE=${{matrix.built_type}} \
              -DCMAKE_CXX_STANDARD=${{matrix.std}} \
              -DCMAKE_CXX_STANDARD_REQUIRED=ON \
              -DCMAKE_CXX_EXTENSIONS=OFF \
              -DRAPIDFUZZ_BUILD_TESTING=1 \
              -DRAPIDFUZZ_ENABLE_LINTERS=1 \
              -DRAPIDFUZZ_BUILD_FUZZERS=1 \
              -G Ninja

    - name: Build
      working-directory: build
      run: ninja

    - name: Test
      working-directory: build
      run: ctest -C ${{matrix.BUILD_TYPE}} --rerun-failed --output-on-failure -j `nproc`

    - name: Fuzz Test
      working-directory: build
      run: |
          fuzzing/fuzz_lcs_similarity -max_total_time=30
          fuzzing/fuzz_levenshtein_distance -max_total_time=30
          fuzzing/fuzz_levenshtein_editops -max_total_time=30
          fuzzing/fuzz_indel_distance -max_total_time=30
          fuzzing/fuzz_indel_editops -max_total_time=30
          fuzzing/fuzz_osa_distance -max_total_time=30
          fuzzing/fuzz_damerau_levenshtein_distance -max_total_time=30"""

