cmake_minimum_required(VERSION 3.16)
project(CarrotFantasy LANGUAGES C CXX)
if(NOT ANDROID)
    message(FATAL_ERROR "This app target is Android-only. See ANDROID.md for build instructions.")
endif()
set(COCOS2DX_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cocos2d")
set(CMAKE_MODULE_PATH "${COCOS2DX_ROOT_PATH}/cmake/Modules")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(CocosBuildSet)
set(BUILD_LUA_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTENSIONS OFF CACHE BOOL "" FORCE)
set(BUILD_EDITOR_SPINE OFF CACHE BOOL "" FORCE)
add_compile_definitions(CC_USE_PHYSICS=0 CC_USE_3D_PHYSICS=0 CC_USE_NAVMESH=0)
add_subdirectory("${COCOS2DX_ROOT_PATH}/cocos" "${ENGINE_BINARY_PATH}/cocos/core")
add_subdirectory("${COCOS2DX_ROOT_PATH}/cocos/platform/android" "${ENGINE_BINARY_PATH}/cocos/android")
target_link_libraries(cpp_android_spec PUBLIC cocos2d)
add_library(carrotfantasy SHARED
    Classes/AppDelegate.cpp Classes/BaseLevelScene.cpp Classes/Carrot.cpp
    Classes/GameManager.cpp Classes/Monster.cpp Classes/MonsterConfigs.cpp
    Classes/Obstacle.cpp Classes/Tower.cpp Classes/advantureScene.cpp
    Classes/menuScene.cpp Classes/seeScene.cpp Classes/setScene.cpp Classes/themeScene.cpp
    proj.android/app/src/main/cpp/main.cpp)
target_include_directories(carrotfantasy PRIVATE Classes)
target_link_libraries(carrotfantasy PRIVATE
    "-Wl,--whole-archive" cpp_android_spec "-Wl,--no-whole-archive" cocos2d)
# Static libc++ plus 16 KB ELF alignment support modern Android page sizes.
target_link_options(carrotfantasy PRIVATE "-Wl,-z,max-page-size=16384" "-Wl,-z,common-page-size=16384")
