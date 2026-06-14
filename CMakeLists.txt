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

# --- Parameters library --------------------------------------------------
set(PARAMETERS_DIR ${CMAKE_SOURCE_DIR}/third_party)
set(PARAMETERS_PREBUILT ${PARAMETERS_DIR}/libparameters.so)

if(EXISTS ${PARAMETERS_PREBUILT})
    message(STATUS "Parameters: linking prebuilt ${PARAMETERS_PREBUILT}")
    # IMPORTED target = "this library already exists", do not build it.
    add_library(parameters SHARED IMPORTED)
    set_target_properties(parameters PROPERTIES
            IMPORTED_LOCATION ${PARAMETERS_PREBUILT}
            INTERFACE_INCLUDE_DIRECTORIES ${PARAMETERS_DIR})
else()
    message(STATUS "Parameters: compiling from source (Parameters.cpp)")
    add_library(parameters STATIC ${PARAMETERS_DIR}/Parameters.cpp)
    target_include_directories(parameters PUBLIC ${PARAMETERS_DIR})
endif()

set(SOURCES
        src/main.cpp
        src/Utils.cpp
        src/GraphGenerator.cpp
)

add_executable(graphs ${SOURCES})

target_include_directories(graphs PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(graphs PRIVATE parameters)

if(EXISTS ${PARAMETERS_PREBUILT})
    set_target_properties(graphs PROPERTIES BUILD_RPATH ${PARAMETERS_DIR})
endif()
