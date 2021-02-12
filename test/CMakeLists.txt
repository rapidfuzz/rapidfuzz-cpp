find_package(Catch2 REQUIRED)

enable_testing()

function(rapidfuzz_add_test NAME SOURCE)
	add_executable(test_${NAME} tests-main.cpp ${SOURCE})
	target_link_libraries(test_${NAME} ${PROJECT_NAME})
	target_link_libraries(test_${NAME} Catch2::Catch2)
	add_test(NAME ${NAME} COMMAND test_${NAME})
endfunction()  

rapidfuzz_add_test(fuzz tests-fuzz.cpp)
rapidfuzz_add_test(levenshtein tests-levenshtein.cpp)
rapidfuzz_add_test(common tests-common.cpp)
