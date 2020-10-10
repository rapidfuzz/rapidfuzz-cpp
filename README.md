  <h1 align="center">
<img src="https://raw.githubusercontent.com/maxbachmann/rapidfuzz/master/docs/img/RapidFuzz.svg?sanitize=true" alt="RapidFuzz" width="400">
</h1>
<h4 align="center">Rapid fuzzy string matching in C++ using the Levenshtein Distance</h4>

<p align="center">
  <a href="https://github.com/maxbachmann/rapidfuzz/actions">
    <img src="https://github.com/maxbachmann/rapidfuzz/workflows/Build/badge.svg"
         alt="Continous Integration">
  </a>
  <a href="https://gitter.im/rapidfuzz/community">
    <img src="https://badges.gitter.im/rapidfuzz/community.svg"
         alt="Gitter chat">
  </a>
  <a href="https://maxbachmann.github.io/rapidfuzz">
    <img src="https://img.shields.io/badge/-documentation-blue"
         alt="Documentation">
  </a>
  <a href="https://github.com/maxbachmann/rapidfuzz/blob/dev/LICENSE">
    <img src="https://img.shields.io/github/license/maxbachmann/rapidfuzz"
         alt="GitHub license">
  </a>
</p>

<p align="center">
  <a href="#description">Description</a> •
  <a href="#installation">Installation</a> •
  <a href="#usage">Usage</a> •
  <a href="#license">License</a>
</p>

---
## Description
RapidFuzz is a fast string matching library for Python and C++, which is using the string similarity calculations from [FuzzyWuzzy](https://github.com/seatgeek/fuzzywuzzy). However there are two aspects that set RapidFuzz apart from FuzzyWuzzy:
1) It is MIT licensed so it can be used whichever License you might want to choose for your project, while you're forced to adopt the GPL license when using FuzzyWuzzy
2) It is mostly written in C++ and on top of this comes with a lot of Algorithmic improvements to make string matching even faster, while still providing the same results. More details on these performance improvements in form of benchmarks can be found [here](https://github.com/maxbachmann/rapidfuzz/blob/master/Benchmarks.md)

The Library is splitted across multiple repositories for the different supported programming languages:
- The C++ version is versioned in this repository
- The Python version can be found at [maxbachmann/rapidfuzz](https://github.com/maxbachmann/rapidfuzz)


## Compilation

RadidFuzz now support CMake.
To build it you can do :

    git clone https://github.com/maxbachmann/rapidfuzz-cpp.git rapidfuzz-cpp
    cd rapidfuzz-cpp
    mkdir build && cd build
    cmake ..
    cmake --build .
    cmake --build . --target install

RapidFuzz exports its targets to CMake. 
You can easily integrate it in your CMake project with 3 options.

1. include it as a subdirectory:
Clone this repo (or make a copy) into your project source tree, lets say in `3rdparty/RapidFuzz`folder.
Then, in your `CMakeLists.txt` use :

    add_subdirectory(rapidfuzz-cpp)
    add_executable(foo main.cpp)
    target_link_libraries(foo rapidfuzz::rapidfuzz)  

2. build it at configure time with FetchContent:
    
    FetchContent_Declare( 
      rapidfuzz
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/rapidfuzz-cpp
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rapidfuzz
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> "${CMAKE_OPT_ARGS}"
    )
    FetchContent_MakeAvailable(rapidfuzz)
    add_executable(foo main.cpp)
    target_link_libraries(foo PRIVATE rapidfuzz::rapidfuzz)

3. use find_package(rapifuzz) if you install it already

    find_package(rapidfuzz REQUIRED)
    add_executable(foo main.cpp)
    target_link_libraries(foo rapidfuzz::rapidfuzz)

## CMake option 

There are 3 CMake options available :
`BUILD_TESTS` : to build test (default OFF)
`BUILD_STATIC` : to build a static library (default ON)
`BUILD_SHARED` : to build a shared library (default ON), which produce the target `rapidfuzz::rapidfuzz-static`

## Usage
```cpp
#include "rapidfuzz/fuzz.hpp"
#include "rapidfuzz/process.hpp"
#include "rapidfuzz/utils.hpp"
```

### Simple Ratio
```cpp
using rapidfuzz::fuzz::ratio;

// score is 96.55171966552734
double score = rapidfuzz::fuzz::ratio("this is a test", "this is a test!");
```

### Partial Ratio
```cpp
// score is 100
double score = rapidfuzz::fuzz::partial_ratio("this is a test", "this is a test!");
```

### Token Sort Ratio
```cpp
// score is 90.90908813476562
double score = rapidfuzz::fuzz::ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a bear")

// score is 100
double score = rapidfuzz::fuzz::token_sort_ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a bear")
```

### Token Set Ratio
```cpp
// score is 83.8709716796875
double score = rapidfuzz::fuzz::token_sort_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")

// score is 100
double score = rapidfuzz::fuzz::token_set_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")
```

### Process
```cpp
// matches is a vector of std::pairs
// [('new york jets', 100), ('new york giants', 78.57142639160156)]
auto matches = rapidfuzz::process::extract(
  "new york jets",
  std::vector<std::string>{"Atlanta Falcons", "New York Jets", "New York Giants", "Dallas Cowboys"},
  rapidfuzz::utils::default_process<std::string>,
  rapidfuzz::fuzz::ratio<std::string, std::string>,
  2);


// matches is a rapidfuzz::optional<std::pair>
// ("dallas cowboys", 90)
auto matches = rapidfuzz::process::extractOne(
  "cowboys",
  std::vector<std::string>{"Atlanta Falcons", "New York Jets", "New York Giants", "Dallas Cowboys"});
```

## License
RapidFuzz is licensed under the MIT license since I believe that everyone should be able to use it without being forced to adopt the GPL license. Thats why the library is based on an older version of fuzzywuzzy that was MIT licensed as well.
This old version of fuzzywuzzy can be found [here](https://github.com/seatgeek/fuzzywuzzy/tree/4bf28161f7005f3aa9d4d931455ac55126918df7).
