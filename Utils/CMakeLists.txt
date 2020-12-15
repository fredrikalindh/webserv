cmake_minimum_required(VERSION 3.8)

set(This Utils)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
    include/Utils/File.hpp
    include/Utils/Encoding.hpp
   # include/Utils/Utf8.hpp
    include/Utils/Uri.hpp
    include/Utils/String.hpp
    include/Utils/Unique_ptr.hpp
    include/Utils/Mutex.hpp
)
set(Sources
    src/File.cpp
    src/Encoding.cpp
  #  src/Utf8.cpp
    src/Uri.cpp
    src/String.cpp
    src/Mutex.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES 
    FOLDER Libraries
)
target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PUBLIC
  zlibstatic
)

if(TEST)
add_subdirectory(test)
endif()
