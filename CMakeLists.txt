cmake_minimum_required(VERSION 3.13)
project(MyProject VERSION 1.0 LANGUAGES C CXX)


set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


include_directories(${CMAKE_SOURCE_DIR}/include)


option(BUILD_SERVER "Build the server" ON)
option(BUILD_CLIENT "Build the client" ON)


file(GLOB_RECURSE SERVER_SOURCES ${CMAKE_SOURCE_DIR}/src/server/*.c ${CMAKE_SOURCE_DIR}/src/server/*.cpp ${CMAKE_SOURCE_DIR}/src/server/*.cc)
file(GLOB_RECURSE CLIENT_SOURCES ${CMAKE_SOURCE_DIR}/src/client/*.c ${CMAKE_SOURCE_DIR}/src/client/*.cpp ${CMAKE_SOURCE_DIR}/src/client/*.cc)


if(MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()


if(BUILD_SERVER)
    add_executable(server ${SERVER_SOURCES})
    target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(server PRIVATE)
endif()

if(BUILD_CLIENT)
    add_executable(client ${CLIENT_SOURCES})
    target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(client PRIVATE)
endif()

