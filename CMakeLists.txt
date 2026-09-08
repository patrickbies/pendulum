cmake_minimum_required(VERSION 3.28)

project(PhysicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(PhysicsEngine ${SOURCES})

target_include_directories(
    PhysicsEngine
    PRIVATE
    src
)

target_link_libraries(
    PhysicsEngine
    PRIVATE
    SDL3::SDL3
)