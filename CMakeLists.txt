cmake_minimum_required(VERSION 2.8)
project(klex)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_subdirectory(src)

# cmake -DGTEST_ROOT=/usr/include/gtest
if (DEFINED GTEST_ROOT)
    add_subdirectory(${GTEST_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/gtest)
    add_subdirectory(test)
    enable_testing()
    add_test(NAME klex-unit-tests COMMAND klex-unit-tests)
endif()

    
