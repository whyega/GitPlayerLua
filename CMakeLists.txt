cmake_minimum_required(VERSION 3.14)

project(GifPlayerLua LANGUAGES CXX)
set(CMAKE_SHARED_LIBRARY_PREFIX "../")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(gifplayer)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gifplayer)

add_subdirectory(SRSignal)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/SRSignal)

add_subdirectory(llmo)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/llmo)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sol2/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lua)

add_library(GifPlayerLua SHARED
  GifPlayerLua_global.h
  GifPlayerLua.cpp
  GifPlayerLua.h

  DrawHook.cpp
  DrawHook.h
)

target_link_libraries(GifPlayerLua  GifPlayer lua51 llmo)

target_compile_definitions(GifPlayerLua PRIVATE GIFPLAYERLUA_LIBRARY)

find_program(UPX_BIN upx)
if(UPX_BIN)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${UPX_BIN} -9 $<TARGET_FILE:${PROJECT_NAME}>)
endif()
