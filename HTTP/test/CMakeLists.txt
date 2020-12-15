cmake_minimum_required(VERSION 3.8)

set(This HTTPTests)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Sources
    src/HttpBuilderTests.cpp
    src/RequestTests.cpp
    src/RequestHandlerTests.cpp
    src/ResponseTests.cpp
    # src/StatusCodesTests.cpp
    src/HeadersTests.cpp
    src/ChunkedBodyTests.cpp
)

add_executable(${This} ${Sources})

set_target_properties(${This} PROPERTIES
        FOLDER Tests
)

target_link_libraries(${This} PUBLIC
    gtest_main
    HTTP
)

add_test(
    NAME ${This}
    COMMAND ${This}
)
