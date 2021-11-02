project(TraCIWildau CXX)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_PREFIX_PATH "$ENV{SUMO_HOME}")
find_package(SUMO REQUIRED)
add_executable(TraCIWildau TraCI-Wildau.cpp tinyxml/tinystr.cpp tinyxml/tinystr.h tinyxml/tinyxml.cpp tinyxml/tinyxmlerror.cpp tinyxml/tinyxml.h tinyxml/tinyxmlparser.cpp)
target_link_libraries(TraCIWildau SUMO::libtracicpp)
