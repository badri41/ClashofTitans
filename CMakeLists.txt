cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# SFML 3.x via FetchContent
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Audio Network)

add_executable(ClashOfTitans
    src/main.cpp
    src/Player.cpp
    src/Bullet.cpp
    src/Jets.cpp
    src/HUD.cpp
)

target_include_directories(ClashOfTitans PRIVATE ${CMAKE_SOURCE_DIR}/Include)

target_compile_features(ClashOfTitans PRIVATE cxx_std_17)
target_link_libraries(ClashOfTitans PRIVATE SFML::Graphics SFML::Audio SFML::Network)
