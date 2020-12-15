cmake_minimum_required(VERSION 3.8)

set(This webserv)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

option(TEST "googletest" OFF)


if ((TEST) AND NOT (EXISTS ${CMAKE_SOURCE_DIR}/googletest)) 
execute_process(COMMAND git clone https://github.com/google/googletest.git ${CMAKE_SOURCE_DIR}/googletest)
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/zlib) 
execute_process(COMMAND git clone https://github.com/madler/zlib.git ${CMAKE_SOURCE_DIR}/zlib)
endif()



  set(ZLIB_EXAMPLES OFF CACHE BOOL "Build zlib example programs" FORCE)
    add_subdirectory(zlib)
    foreach(Child zlib zlibstatic)
        set_target_properties(${Child} PROPERTIES FOLDER Libraries/zlib)
        target_include_directories(${Child} INTERFACE $<TARGET_PROPERTY:zlib,SOURCE_DIR> $<TARGET_PROPERTY:zlib,BINARY_DIR>)
    endforeach()


if (TEST)
add_subdirectory(googletest)
endif()


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(Sources
    webserv.cpp
)

set(CLANG_WARNINGS
    -Wall 
    -Werror 
    -Wextra 
)

add_compile_options(${CLANG_WARNINGS})

add_subdirectory(Utils)
add_subdirectory(Config)
add_subdirectory(HTTP)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    Threads::Threads
    Utils
    Config
    HTTP
)

