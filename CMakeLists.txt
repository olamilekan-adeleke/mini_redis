cmake_minimum_required(VERSION 3.10)
project(mini_redis)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(include/nework)
include_directories(include/server)
include_directories(include/parser)
include_directories(include/command)

# Automatically detect all .cpp files in src/ and lib/ (now and future)
file(GLOB_RECURSE SOURCES 
    "src/*.cpp" 
    "src/nework/*.cpp" 
    "src/server/*.cpp" 
    "src/parser/*.cpp"
    "src/command/*.cpp"
)

# Build executable from all detected sources
add_executable(app ${SOURCES})

# Run target (auto-updates when sources change)
add_custom_target(run
    COMMAND ./app
    DEPENDS app
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
