cmake_minimum_required(VERSION 2.8.12)
project(bpistats)
set(SOURCES src/bpistats.cpp)
find_package(Boost 1.53.0 REQUIRED COMPONENTS unit_test_framework)

# Main executable
add_executable(bpistats ${SOURCES})
target_include_directories(bpistats PUBLIC include)
#target_link_libraries(bpistats PUBLIC Boost::boost)

# Unit tests
enable_testing()
add_executable(stat_selector_test test/stat_selector_test.cpp)
target_include_directories(stat_selector_test PUBLIC include)
target_link_libraries(stat_selector_test PUBLIC boost_unit_test_framework)
target_compile_definitions(stat_selector_test PRIVATE BOOST_TEST_DYN_LINK)
add_test(test_stat_selector stat_selector_test)

