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


## Installation
As of now it it only possible to use the sources directly by adding them to your project. There will be a version on conan in the future.


## Usage
```cpp
#include "fuzz.hpp"
#include "process.hpp"
```

### Simple Ratio
```cpp
using rapidfuzz::fuzz::ratio;

// score is 96.55171966552734
double score = rapidfuzz::fuzz::ratio("this is a test", "this is a test!");
```

### Partial Ratio
```cpp
using rapidfuzz::fuzz::partial_ratio;

// score is 100
double score = partial_ratio("this is a test", "this is a test!");
```

### Token Sort Ratio
```cpp
using rapidfuzz::fuzz::ratio;
using rapidfuzz::fuzz::token_sort_ratio;

// score is 90.90908813476562
double score = ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a bear")

// score is 100
double score = token_sort_ratio("fuzzy wuzzy was a bear", "wuzzy fuzzy was a bear")
```

### Token Set Ratio
```cpp
using rapidfuzz::fuzz::token_sort_ratio;
using rapidfuzz::fuzz::token_set_ratio;

// score is 83.8709716796875
double score = token_sort_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")

// score is 100
double score = token_set_ratio("fuzzy was a bear", "fuzzy fuzzy was a bear")
```

### Process
```cpp
#include "process.hpp"
using rapidfuzz::process::extract;
using rapidfuzz::process::extractOne;

// matches is a vector of std::pairs
// [('new york jets', 100), ('new york giants', 78.57142639160156)]
auto matches = extract(
  "new york jets",
  std::vector<std::string>{"Atlanta Falcons", "New York Jets", "New York Giants", "Dallas Cowboys"},
  utils::default_process<char>,
  fuzz::ratio<std::string, std::string>
  2);


// matches is a boost::optional<std::pair>
// ("dallas cowboys", 90)
auto matches = extractOne(
  "cowboys",
  std::vector<std::string>{"Atlanta Falcons", "New York Jets", "New York Giants", "Dallas Cowboys"});
```

## License
RapidFuzz is licensed under the MIT license since I believe that everyone should be able to use it without being forced to adopt the GPL license. Thats why the library is based on an older version of fuzzywuzzy that was MIT licensed as well.
This old version of fuzzywuzzy can be found [here](https://github.com/seatgeek/fuzzywuzzy/tree/4bf28161f7005f3aa9d4d931455ac55126918df7).
