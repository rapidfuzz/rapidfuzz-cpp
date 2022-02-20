foreach(fuzzer levenshtein_distance levenshtein_editops)
  add_executable(fuzz_${fuzzer} fuzz_${fuzzer}.cpp)
  target_compile_features(fuzz_${fuzzer} PUBLIC cxx_std_14)
  target_link_libraries(fuzz_${fuzzer} PRIVATE rapidfuzz::rapidfuzz)

  target_compile_options(fuzz_${fuzzer} PRIVATE -g -O1 -fsanitize=fuzzer)
  target_link_libraries(fuzz_${fuzzer} PRIVATE -fsanitize=fuzzer)
endforeach()