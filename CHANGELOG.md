## Changelog

### [1.3.0] - 2022-
#### Performance
- improve performance of Levenshtein distance
  - improve performance when `score_cutoff = 1`
  - improve performance for long sequences when `3 < score_cutoff < 32`
- improve performance of Levenshtein editops

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
