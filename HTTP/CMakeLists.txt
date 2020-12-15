cmake_minimum_required(VERSION 3.8)

set(This HTTP)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
    include/HTTP/BufferIn.hpp
    include/HTTP/BufferOut.hpp
    include/HTTP/IFileIO.hpp
    include/HTTP/SocketIO.hpp
    include/HTTP/Server.hpp
    include/HTTP/CGI.hpp
    include/HTTP/HttpBuilder.hpp
    include/HTTP/Request.hpp
    include/HTTP/RequestHandler.hpp
    include/HTTP/Response.hpp
    include/HTTP/StatusCodes.hpp
    include/HTTP/Headers.hpp
    include/HTTP/ChunkedBody.hpp
)
set(Sources
    src/BufferIn.cpp
    src/BufferOut.cpp
    src/SocketIO.cpp
    src/Server.cpp
    src/CGI.cpp
    src/HttpBuilder.cpp
    src/Request.cpp
    src/RequestHandler.cpp
    src/Response.cpp
    src/StatusCodes.cpp
    src/Headers.cpp
    src/ChunkedBody.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

set_target_properties(${This} PROPERTIES 
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

target_link_libraries(${This} PUBLIC
    Utils
    Config
)

if(TEST)
add_subdirectory(test)
endif()
