find_package(Catch2 3 REQUIRED)

function(rapidfuzz_add_test NAME SOURCE)
	add_executable(test_${NAME} ${SOURCE})
	target_link_libraries(test_${NAME} ${PROJECT_NAME})
	target_link_libraries(test_${NAME} Catch2::Catch2WithMain )
	add_test(NAME ${NAME} COMMAND test_${NAME})
endfunction()

rapidfuzz_add_test(Hamming tests-Hamming.cpp)
rapidfuzz_add_test(Levenshtein tests-Levenshtein.cpp)
