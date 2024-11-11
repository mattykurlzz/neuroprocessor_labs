cmake_minimum_required(VERSION 3.19)
project(neuro-model)

if(NOT WIN32 AND NOT UNIX)
  message(FATAL_ERROR "Only WIN32 and UNIX platforms are currently supported")
endif()

set(SYSTEMC_VER "2.3.3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
find_package(SystemC)
if(SystemC_FOUND)
  include_directories(${SystemC_INCLUDE_DIRS})
  link_directories(${SystemC_LIBRARY_DIRS})
  set(ENV{SYSTEMC_INCLUDE} ${SystemC_INCLUDE_DIRS})
  set(ENV{SYSTEMC_LIBDIR} ${SystemC_LIBRARY_DIRS})
endif()

# Create bin
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/bin)
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif()

# Add SystemC as subdirectory
if(NOT SystemC_FOUND) 
  message(STATUS "Adding local SystemC from ${CMAKE_SOURCE_DIR}/ext/systemc-${SYSTEMC_VER}")
  add_subdirectory(${CMAKE_SOURCE_DIR}/ext/systemc-${SYSTEMC_VER})
endif()

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(software/)
add_subdirectory(hardware/)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
                                        hardware 
                                        software)
