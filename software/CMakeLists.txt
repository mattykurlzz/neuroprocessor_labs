FILE (GLOB SOFTWARE_SRC ${PROJECT_SOURCE_DIR}/software/source/*.cpp)

add_library(software STATIC ${SOFTWARE_SRC})

target_include_directories(software PUBLIC ${PROJECT_SOURCE_DIR}/software/include)