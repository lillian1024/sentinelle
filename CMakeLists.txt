cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0146 OLD)
project(Sentinelle VERSION 0.0.1)

include(CTest)
enable_testing()

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

add_executable(Sentinelle src/main.cxx)

target_link_libraries( Sentinelle ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
