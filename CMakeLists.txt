cmake_minimum_required(3.0)
project("Path Finder")

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(pathFinder ${SOURCES})
