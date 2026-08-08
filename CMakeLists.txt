cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0146 OLD)
project(Sentinelle VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++26")
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
    src/core/components/analizers/analizer.cxx
    src/core/components/analizers/modules/utils/analizer-trigger-if-true.cxx
    src/core/components/analizers/modules/utils/analizer-show-image.cxx
    src/core/components/analizers/modules/utils/analizer-save-video.cxx
    src/core/components/analizers/modules/dnn/analizer-dnn.cxx
    src/core/components/analizers/modules/dnn/modules/analizer-dnn-generic-ident.cxx
    src/core/components/analizers/modules/dnn/modules/analizer-dnn-yolov9.cxx
    )

#Core chain sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/core/components/chains/chain.cxx
    src/core/components/chains/context/context.cxx
    src/core/components/chains/analizer-usage.cxx
    src/core/components/chains/path-data.cxx
    )

#Core event sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/core/event/event-manager.cxx
    src/core/event/events/event.cxx
    src/core/event/events/modules/trigger/triggered-event.cxx
    src/core/event/events/modules/trigger/untriggered-event.cxx
    )

#Utils config sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/config/config-manager.cxx
    src/utils/config/data/general-settings.cxx
    src/utils/config/data/sources/sources-settings.cxx
    src/utils/config/data/sources/source-settings.cxx
    src/utils/config/data/sources/source-type/url-source-settings.cxx
    src/utils/config/data/analizers/analizers-settings.cxx
    src/utils/config/data/chains/chains-settings.cxx
    )

#Utils thread sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/thread/thread-manager.cxx
    )

#Utils logger sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/logger/logger.cxx
    )

#Utils opencv sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/opencv/video-stream/video-stream.cxx
    src/utils/opencv/video-stream/live-video-stream.cxx
    src/utils/opencv/video-stream/sequence-video-stream.cxx
    )

#Utils cache sources
set(PROJECT_SRCS ${PROJECT_SRCS}
    src/utils/cache/cache-manager.cxx
    )

add_executable(Sentinelle ${PROJECT_SRCS})

target_include_directories(Sentinelle PRIVATE src)

#Add libraries to the binary
target_link_libraries(Sentinelle ${OpenCV_LIBS})
target_link_libraries(Sentinelle yaml-cpp::yaml-cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
