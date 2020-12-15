cmake_minimum_required(VERSION 3.8)

set(This UtilsTests)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Sources
    src/UriTests.cpp
    # src/Utf8Tests.cpp
    src/StringTests.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
        FOLDER Tests
)

target_link_libraries(${This} PUBLIC
    gtest_main
    Utils
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
