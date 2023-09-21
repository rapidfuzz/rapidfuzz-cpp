find_package(Catch2 3 QUIET)
if (Catch2_FOUND)
    message("Using system supplied version of Catch2")
else()
    message("Using FetchContent to load Catch2")
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
        GIT_TAG         v3.4.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()

if (RAPIDFUZZ_ENABLE_LINTERS)
    # include aminya & jason turner's C++ best practices recommended cmake project utilities
    message("Enable Linters on test build")
    include(FetchContent)

    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.20)
        FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.26.2.zip)
    else()
        FetchContent_Declare(_project_options URL https://github.com/aminya/project_options/archive/refs/tags/v0.25.2.zip)
    endif()
    FetchContent_MakeAvailable(_project_options)
    include(${_project_options_SOURCE_DIR}/Index.cmake)

    project_options(
            # ENABLE_CACHE
            # ENABLE_CONAN
            WARNINGS_AS_ERRORS
            # ENABLE_CPPCHECK
            # ENABLE_CLANG_TIDY
            # ENABLE_INCLUDE_WHAT_YOU_USE
            # ENABLE_COVERAGE
            # ENABLE_PCH
            # PCH_HEADERS <Eigen/Dense> <fmt/core.h> <vector> <utility> <string> <string_view>
            # ENABLE_DOXYGEN
            # ENABLE_IPO
            # ENABLE_USER_LINKER
            # ENABLE_BUILD_WITH_TIME_TRACE
            # ENABLE_UNITY
            # ENABLE_SANITIZER_ADDRESS
            # ENABLE_SANITIZER_LEAK
            # ENABLE_SANITIZER_UNDEFINED_BEHAVIOR
            # ENABLE_SANITIZER_THREAD
            # ENABLE_SANITIZER_MEMORY
            # CLANG_WARNINGS "-Weverything"
    )
endif()

function(rapidfuzz_add_test test)
    add_executable(test_${test} tests-${test}.cpp)
    target_link_libraries(test_${test} ${PROJECT_NAME})
    target_link_libraries(test_${test} Catch2::Catch2WithMain)
    if (RAPIDFUZZ_ENABLE_LINTERS)
        target_link_libraries(test_${test} project_warnings)
    endif()
    add_test(NAME ${test} COMMAND test_${test})
endfunction()

rapidfuzz_add_test(fuzz)
rapidfuzz_add_test(common)

add_subdirectory(distance)
