cmake_minimum_required(VERSION 3.19)
project(neuro-model)

set(SYSTEMC_VER "2.3.3")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_PREFIX_PATH /opt/systemc)

include_directories(${CMAKE_PREFIX_PATH}/include)
find_package(SystemCLanguage CONFIG REQUIRED)
link_directories(${CMAKE_PREFIX_PATH}/lib)

add_subdirectory(software/)
add_subdirectory(hardware/)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE 
                                        hardware 
                                        software
                                        SystemC::systemc)