cmake_minimum_required(VERSION 3.8)

project(test-RE)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include(CodeCoverage)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(
        test_RE
        run_tests.cpp
        TestSM.cpp
        TestRE.cpp
        all_classes.h
        ../src/RegularExpression.cpp ../src/RegularExpression.h
        ../src/SMGraph.cpp ../src/SMGraph.h
        ../src/StateMachine.cpp ../src/StateMachine.h
)

target_link_libraries(test_RE ${GTEST_LIBRARIES} Threads::Threads)

setup_target_for_coverage(tests test_RE coverage)
SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")