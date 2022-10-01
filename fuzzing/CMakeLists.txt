function(create_fuzzer fuzzer)
    add_executable(fuzz_${fuzzer} fuzz_${fuzzer}.cpp)
    target_compile_features(fuzz_${fuzzer} PUBLIC cxx_std_17)
    target_link_libraries(fuzz_${fuzzer} PRIVATE rapidfuzz::rapidfuzz)

    target_compile_options(fuzz_${fuzzer} PRIVATE -g -O1 -fsanitize=fuzzer,address -march=native)
    target_link_libraries(fuzz_${fuzzer} PRIVATE -fsanitize=fuzzer,address)
endfunction(create_fuzzer)

create_fuzzer(lcs_similarity)

create_fuzzer(levenshtein_distance)
create_fuzzer(levenshtein_editops)

create_fuzzer(indel_distance)
create_fuzzer(indel_editops)

create_fuzzer(osa_distance)

create_fuzzer(damerau_levenshtein_distance)

create_fuzzer(jaro_similarity)
