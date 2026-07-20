cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0146 OLD)
project(Sentinelle VERSION 0.0.1)

include(FetchContent)
include(CTest)
enable_testing()

#Included libraries
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG master # Can be a tag (yaml-cpp-x.x.x), a commit hash, or a branch name (master)
)
FetchContent_MakeAvailable(yaml-cpp)

#Project's configuration
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

set(PROJECT_SRCS src/main.cxx)

#Core orchestrator sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/core/orchestrator/orchestrator.cxx)

#Core source sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/core/components/sources/source.cxx)

#Core analizer sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/core/components/analizers/analizer.cxx)

#Utils config sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/config/config-manager.cxx
    src/utils/config/data/general-settings.cxx
    src/utils/config/data/sources/sources-settings.cxx
    src/utils/config/data/sources/source-settings.cxx
    src/utils/config/data/sources/source-type/url-source-settings.cxx
    )

add_executable(Sentinelle ${PROJECT_SRCS})

target_include_directories(Sentinelle PRIVATE src)

#Add libraries to the binary
target_link_libraries(Sentinelle ${OpenCV_LIBS})
target_link_libraries(Sentinelle yaml-cpp::yaml-cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
