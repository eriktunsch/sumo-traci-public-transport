project(TraCIWildau CXX)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_PREFIX_PATH "$ENV{SUMO_HOME}")
find_package(SUMO REQUIRED)
add_executable(TraCIWildau TraCI-Wildau.cpp tinystr.cpp tinystr.h tinyxml.cpp tinyxmlerror.cpp tinyxml.h tinyxmlparser.cpp)
target_link_libraries(TraCIWildau SUMO::libtracicpp)
