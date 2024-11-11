FILE (GLOB HARDWARE_SRC ${PROJECT_SOURCE_DIR}/hardware/source/*.cpp)

add_library(hardware STATIC ${HARDWARE_SRC})

target_include_directories(hardware PUBLIC  ${PROJECT_SOURCE_DIR}/hardware/include
                                            ${PROJECT_SOURCE_DIR}/software/include)