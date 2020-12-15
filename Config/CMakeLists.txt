cmake_minimum_required(VERSION 3.8)

set(This Config)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(Headers
    include/Config/ServerConfig.hpp
    include/Config/Server.hpp
    include/Config/Location.hpp
    include/Config/LocationList.hpp
    include/Config/Parser.hpp
    include/Config/Token.hpp
    include/Config/MimeTypes.hpp
)
set(Sources
    src/Server.cpp
    src/Location.cpp
    src/LocationList.cpp
    src/ServerConfig.cpp
    src/Token.cpp
    src/MimeTypes.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES 
    FOLDER Libraries
)
target_include_directories(${This} PUBLIC include)
target_link_libraries(${This} PUBLIC
   Utils
)
if(TEST)
add_subdirectory(test)
endif()
