cmake_minimum_required(VERSION 2.8.12)
project(bpistats)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")
set(SOURCES src/bpistats.cpp)
find_package(Boost 1.53.0 REQUIRED COMPONENTS unit_test_framework)

# Main executable
add_executable(bpistats ${SOURCES})
target_include_directories(bpistats PUBLIC include)
target_link_libraries(bpistats PUBLIC boost_program_options)

# Unit tests
enable_testing()
add_executable(stat_aggregator_test test/stat_aggregator_test.cpp)
target_include_directories(stat_aggregator_test PUBLIC include)
target_link_libraries(stat_aggregator_test PUBLIC boost_unit_test_framework)
target_compile_definitions(stat_aggregator_test PRIVATE BOOST_TEST_DYN_LINK)
add_test(test_stat_selector stat_aggregator_test)

