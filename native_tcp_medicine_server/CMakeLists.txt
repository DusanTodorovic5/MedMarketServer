cmake_minimum_required(VERSION 3.0.0)
project(medicine_server VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories("src/")
include_directories("hpp/")

file(GLOB files CONFIGURE_DEPENDS
     "*.h"
     "*.cpp"
     "hpp/*.hpp"
     "src/*.cpp"
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(medicine_server ${files})

target_link_libraries(medicine_server Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
