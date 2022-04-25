## Changelog

### [1.0.1] - 2022-04-
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
