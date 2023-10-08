## Changelog

## [2.1.1] - 2023-10-08
### Fixed
- fix edge case in new simd implementation of Jaro and Jaro Winkler

## [2.1.0] - 2023-10-08
### Changed
- add support for bidirectional iterators
- add experimental simd implementation for Jaro and Jaro Winkler

### [2.0.0] - 2023-06-02
#### Changed
- added argument ``pad`` to Hamming distance. This controls whether sequences of different
  length should be padded or lead to a `std::invalid_argument` exception.
- improve behaviour when including the project as cmake sub project

### [1.11.3] - 2023-04-18
#### Fixed
- add missing include leading to build failures on gcc 13

### [1.11.2] - 2023-04-17
#### Fixed
- fix handling of `score_cutoff > 1.0` in `Jaro` and `JaroWinkler`

### [1.11.1] - 2023-04-16
#### Fixed
- fix division by zero in simd implementation of normalized string metrics, when comparing empty strings

### [1.11.0] - 2023-04-16
#### Changed
- allow the usage of hamming for different string lengths. Length differences are handled as
  insertions / deletions

#### Fixed
- fix some floating point comparisions in the test suite

### [1.10.4] - 2022-12-14
#### Changed
- Linters are now disabled in test builds by default and can be enabled using `RAPIDFUZZ_ENABLE_LINTERS`

### [1.10.3] - 2022-12-13
#### Fixed
- fix warning about `project_options` when building the test suite with `cmake>=3.24`

### [1.10.2] - 2022-12-01
#### Fixed
- `fuzz::partial_ratio` was not always symmetric when `len(s1) == len(s2)`
- fix undefined behavior in experimental SIMD implementaton

### [1.10.1] - 2022-11-02
#### Fixed
- fix broken sse2 support

### [1.10.0] - 2022-10-29
#### Fixed
- fix bug in `Levenshtein.editops` leading to crashes when used with `score_hint`

#### Changed
- add `score_hint` argument to cached implementations
- add `score_hint` argument to Levenshtein functions

### [1.9.0] - 2022-10-22
#### Added
- added `Prefix`/`Postfix` similarity

### [1.8.0] - 2022-10-02
#### Fixed
- fixed incorrect score_cutoff handling in `lcs_seq_distance`

#### Added
- added experimental simd support for `ratio`/`Levenshtein`/`LCSseq`/`Indel`
- add Jaro and JaroWinkler

### [1.7.0] - 2022-09-18
#### Added
- add editops to hamming distance

#### Performance
- strip common affix in osa distance

### [1.6.0] - 2022-09-16
#### Added
- add optimal string alignment (OSA) alignment

### [1.5.0] - 2022-09-11
#### Fix
- `fuzz::partial_ratio` did not find the optimal alignment in some edge cases

#### Performance
- improve performance of `fuzz::partial_ratio`

### [1.4.1] - 2022-09-11
#### Fixed
- fix type mismatch error

### [1.4.0] - 2022-09-10
#### Performance
- improve performance of Levenshtein distance/editops calculation for long
  sequences when providing a `score_cutoff`/`score_hint`

### [1.3.0] - 2022-09-03
#### Performance
- improve performance of Levenshtein distance
  - improve performance when `score_cutoff = 1`
  - improve performance for long sequences when `3 < score_cutoff < 32`
- improve performance of Levenshtein editops

#### Fixed
- fix incorrect results of partial_ratio for long needles

### [1.2.0] - 2022-08-20
#### Added
- added damerau levenshtein implementation
  - Not API stable yet, since it will be extended with weights in a future version

### [1.1.1] - 2022-07-29
#### Performance
- improve performance for banded Levenshtein implementation

### [1.1.0] - 2022-07-29
#### Fixed
- fix banded Levenshtein implementation

#### Changed
- implement Hirschbergs algorithms to reduce memory usage of
  levenshtein_editops

### [1.0.5] - 2022-07-23
#### Fixed
- fix opcode conversion for empty source sequence

### [1.0.4] - 2022-06-29
#### Fixed
- fix implementation of hamming_normalized_similarity
- fix implementation of CachedLCSseq::distance

### [1.0.3] - 2022-06-24
#### Fixed
- fix integer wraparound in partial_ratio/partial_ratio_alignment

### [1.0.2] - 2022-06-11
#### Fixed
- fix unlimited recursion in CachedLCSseq::similarity
- reduce compiler warnings

### [1.0.1] - 2022-04-16
#### Fixed
- fix undefined behavior in sorted_split incrementing iterator past the end
- fix use after free in editops calculation
- reduce compiler warnings

### [1.0.1] - 2022-04-16
#### Added
- added LCSseq (longest common subsequence) implementation

#### Fixed
- reduced compiler warnings
- consider float imprecision in score_cutoff
- fix incorrect score_cutoff handling in token_set_ratio and token_ratio
- fix template deduction guides on MSVC
