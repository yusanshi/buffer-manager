cmake_minimum_required(VERSION 3.12)
project(buffer-manager VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CXXOPTS_PATH ${CMAKE_SOURCE_DIR}/src/cxxopts.h)

if (NOT EXISTS ${CXXOPTS_PATH})
    file(DOWNLOAD
    https://raw.githubusercontent.com/jarro2783/cxxopts/c74846a891b3cc3bfa992d588b1295f528d43039/include/cxxopts.hpp
    ${CXXOPTS_PATH}
    SHOW_PROGRESS)
endif()

file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS 
  src/*.cc
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(manager ${SOURCE})
target_link_libraries(manager Threads::Threads)

include_directories(src)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
