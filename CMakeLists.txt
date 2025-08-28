cmake_minimum_required(VERSION 3.14)

project(aht10 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)

# Create a library for the aht10 sensor
add_library(aht10_driver src/aht10.c)
target_include_directories(aht10_driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Add CppUTest Submodule ---
add_subdirectory(lib/cpputest)

# Test executable
add_executable(run_tests tests/aht10_test.cpp)

target_link_libraries(run_tests PRIVATE aht10_driver CppUTest CppUTestExt)

# --- CTest Integration ---
# Enable and add the test so we can run it with 'make test'.
enable_testing()
add_test(NAME aht10_unit_tests COMMAND run_tests)