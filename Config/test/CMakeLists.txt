cmake_minimum_required(VERSION 3.8)

set(This ConfigTests)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Sources
    src/ServerTests.cpp
    src/LocationTests.cpp
    src/LocationListTests.cpp
    src/ParserTests.cpp
    # src/ListenTests.cpp
    src/MimeTypesTests.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
        FOLDER Tests
)

target_link_libraries(${This} PUBLIC
    gtest_main
    Config
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
