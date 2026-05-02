cmake_minimum_required(VERSION 3.10)
project(GraphAlgorithms VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

set(SOURCES
        src/main.cpp
        src/Utils.cpp
        src/GraphGenerator.cpp
        # third_party/parser.cpp
)

add_executable(graphs ${SOURCES})

target_include_directories(graphs PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        # ${CMAKE_SOURCE_DIR}/third_party/parser
)