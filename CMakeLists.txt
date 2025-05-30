cmake_minimum_required(VERSION 3.10)
project(mini_redis)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/nework)
include_directories(include/server)
include_directories(include/parser)
include_directories(include/command)
include_directories(include/utils)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/nework/*.cpp"
    "src/server/*.cpp"
    "src/parser/*.cpp"
    "src/command/*.cpp"
    "src/utils/*.cpp"
)


# Build type configuration
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Building in Debug mode")
    
    # Set common debug flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    
    # Compiler-specific debug flags
    if(MSVC)
        # MSVC (Visual Studio) flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address /Zi")
        # Link flags for ASAN on MSVC
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
    else()
        # GCC/Clang flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
        # Link flags for ASAN on GCC/Clang
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    endif()
    
    set(BUILD_POSTFIX "_debug") # Add _debug to the executable name
else() 
    message(STATUS "Building in ${CMAKE_BUILD_TYPE} mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") # Optimization flags
    set(BUILD_POSTFIX "")
endif()

# FetchContent for managing external dependencies
include(FetchContent)

# Fetch Boost from GitHub
set(BOOST_VERSION 1.83.0)
string(REPLACE "." "_" BOOST_VERSION_UNDERSCORE ${BOOST_VERSION})

FetchContent_Declare(
    boost
    URL https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    # Alternatively, you can use the git repository
    # GIT_REPOSITORY https://github.com/boostorg/boost.git
    # GIT_TAG boost-${BOOST_VERSION}
)

# Configure Boost build options
set(BOOST_INCLUDE_LIBRARIES fiber context)
set(BOOST_ENABLE_CMAKE ON)
set(BUILD_SHARED_LIBS OFF) # Build static libraries

# Make Boost available
FetchContent_MakeAvailable(boost)

# Create the executable
add_executable(app${BUILD_POSTFIX} ${SOURCES})

# Link with Boost libraries
target_link_libraries(app${BUILD_POSTFIX} 
    Boost::fiber
)

# Run target
add_custom_target(run
    COMMAND ./app${BUILD_POSTFIX}
    DEPENDS app${BUILD_POSTFIX} 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Set default build type
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE) 
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Installation
install(TARGETS app${BUILD_POSTFIX} DESTINATION bin)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    if(NOT MSVC)
        # For GCC/Clang, ensure the runtime finds the sanitizer library
        set_target_properties(app${BUILD_POSTFIX} PROPERTIES
            ENVIRONMENT "ASAN_OPTIONS=detect_leaks=1:symbolize=1")
    endif()
endif()
