cmake_minimum_required(VERSION 3.7.1)
project(path_finder)

include_directories(include)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES "src/*.cpp")

add_executable(path_finder ${SOURCES})

include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} SDLShapes)


