cmake_minimum_required(VERSION 3.10)
project(GraphAlgorithms VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# default to -O3
# unless the user explicitly asks for another build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(parameters STATIC third_party/Parameters.cpp)
target_include_directories(parameters PUBLIC ${CMAKE_SOURCE_DIR}/third_party)

set(SOURCES
        src/main.cpp
        src/Utils.cpp
        src/GraphGenerator.cpp
)

add_executable(graphs ${SOURCES})

target_include_directories(graphs PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(graphs PRIVATE parameters)
