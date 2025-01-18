name: CMake

on: [push, pull_request]

env:
  BUILD_TYPE: Release

jobs:
  build_linux_clang:
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        BUILD_TYPE: [Release, Debug]

    steps:
    - uses: actions/checkout@v2

    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} -DRAPIDFUZZ_BUILD_TESTING=1 -DRAPIDFUZZ_ENABLE_LINTERS=1 -DRAPIDFUZZ_BUILD_FUZZERS=1 -DCMAKE_CXX_COMPILER=clang++

    - name: Build
      run: cmake --build build --config ${{matrix.BUILD_TYPE}}

    - name: Test
      working-directory: build
      run: ctest -C ${{matrix.BUILD_TYPE}} --rerun-failed --output-on-failure

    - name: Fuzz Test
      working-directory: build
      run: |
          fuzzing/fuzz_lcs_similarity -max_total_time=30
          fuzzing/fuzz_levenshtein_distance -max_total_time=30
          fuzzing/fuzz_levenshtein_editops -max_total_time=30
          fuzzing/fuzz_indel_distance -max_total_time=30
          fuzzing/fuzz_indel_editops -max_total_time=30
          fuzzing/fuzz_osa_distance -max_total_time=30
          fuzzing/fuzz_damerau_levenshtein_distance -max_total_time=30

  build_linux_clang_32:
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        BUILD_TYPE: [Release, Debug]
    env:
        CXXFLAGS: -m32
        CFLAGS: -m32

    steps:
    - uses: actions/checkout@v2

    - name: Install Dependencies
      run: |
        sudo apt update
        sudo apt install -y libc6-dev-i386 g++-multilib

    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} -DRAPIDFUZZ_BUILD_TESTING=1 -DRAPIDFUZZ_ENABLE_LINTERS=1 -DRAPIDFUZZ_BUILD_FUZZERS=1 -DCMAKE_CXX_COMPILER=clang++

    - name: Build
      run: cmake --build build --config ${{matrix.BUILD_TYPE}}

    - name: Test
      working-directory: build
      run: ctest -C ${{matrix.BUILD_TYPE}} --rerun-failed --output-on-failure

    - name: Fuzz Test
      working-directory: build
      run: |
          fuzzing/fuzz_lcs_similarity -max_total_time=30
          fuzzing/fuzz_levenshtein_distance -max_total_time=30
          fuzzing/fuzz_levenshtein_editops -max_total_time=30
          fuzzing/fuzz_indel_distance -max_total_time=30
          fuzzing/fuzz_indel_editops -max_total_time=30
          fuzzing/fuzz_osa_distance -max_total_time=30
          fuzzing/fuzz_damerau_levenshtein_distance -max_total_time=30

  build_linux_gcc:
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        BUILD_TYPE: [Release, Debug]

    steps:
      - uses: actions/checkout@v2

      - name: Configure CMake
        run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}} -DRAPIDFUZZ_BUILD_TESTING=1 -DRAPIDFUZZ_ENABLE_LINTERS=1 -DCMAKE_CXX_COMPILER=g++

      - name: Build
        run: cmake --build build --config ${{matrix.BUILD_TYPE}}

      - name: Test
        working-directory: build
        run: ctest -C ${{matrix.BUILD_TYPE}} --rerun-failed --output-on-failure

  build_windows:
    runs-on: windows-latest
    strategy:
      fail-fast: false
      matrix:
        BUILD_TYPE: [Release, Debug]

    steps:
    - uses: actions/checkout@v2

    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}}  -DRAPIDFUZZ_BUILD_TESTING=1 -DRAPIDFUZZ_ENABLE_LINTERS=1

    - name: Build
      run: cmake --build build --config ${{matrix.BUILD_TYPE}}

    - name: Test
      working-directory: build
      run: ctest -C ${{matrix.BUILD_TYPE}} --rerun-failed --output-on-failure

  build_cmake_installed:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release

    - name: Install RapidFuzz
      run: sudo cmake --build build --target install

    - name: Configure example project
      working-directory: examples/cmake_installed
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release

    - name: Build example project
      working-directory: examples/cmake_installed
      run: cmake --build build --config ${{env.BUILD_TYPE}}

    - name: Run example project
      working-directory: examples/cmake_installed/build
      run: ./foo

  build_cmake_subdir:
    runs-on: ubuntu-latest
    strategy:
      fail-fast: false
      matrix:
        BUILD_TYPE: [Release, Debug]

    steps:
    - uses: actions/checkout@v2

    - name: Configure the library dependent on RapidFuzz
      working-directory: examples/cmake_export
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}}

    - name: Build the library dependent on RapidFuzz
      working-directory: examples/cmake_export
      run: cmake --build build --config ${{matrix.BUILD_TYPE}}

    - name: Install the library dependent on RapidFuzz
      working-directory: examples/cmake_export
      run: sudo cmake --build build --target install

    - name: Configure the app indirectly dependent on RapidFuzz
      working-directory: examples/cmake_export/indirect_app
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{matrix.BUILD_TYPE}}

    - name: Build the app indirectly dependent on RapidFuzz
      working-directory: examples/cmake_export/indirect_app
      run: cmake --build build --config ${{matrix.BUILD_TYPE}}

    - name: Run the app indirectly dependent on RapidFuzz
      working-directory: examples/cmake_export/indirect_app/build
      run: ./fooapp

  build_cpack_installed:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v2

    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release

    - name: Install RapidFuzz
      working-directory: build
      run: |
          cpack -G DEB
          sudo dpkg -i *.deb

    - name: Configure example project
      working-directory: examples/cmake_installed
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release

    - name: Build example project
      working-directory: examples/cmake_installed
      run: cmake --build build --config ${{env.BUILD_TYPE}}

    - name: Run example project
      working-directory: examples/cmake_installed/build
      run: ./foo
