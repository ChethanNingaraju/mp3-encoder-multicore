project(encoder-multi-core)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/lame_prebuilt/libmp3lame.dll pthread)
else()
    message("Assuming Linux platform")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/external/lame-3.100/build_linux/lib/libmp3lame.a pthread)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ./src/include)
target_include_directories(${PROJECT_NAME} PRIVATE external/lame-3.100/include)


