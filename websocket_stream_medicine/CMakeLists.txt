cmake_minimum_required(VERSION 3.0.0)
project(medicine VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("src/")
include_directories("hpp/")


find_package(yaml-cpp)

file(GLOB files CONFIGURE_DEPENDS
     "*.h"
     "*.cpp"
     "hpp/*.hpp"
     "src/*.cpp"
) 

add_executable(medicine ${files})

target_link_libraries(medicine
    yaml-cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
