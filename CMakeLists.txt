project(encoder-multi-core)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/lame_prebuilt/libmp3lame.dll pthread)
target_include_directories(${PROJECT_NAME} PRIVATE ./src/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/lame-3.100/include)


