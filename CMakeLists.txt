cmake_minimum_required(VERSION 3.20)
project(Path_to_Home_game LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Path_to_Home_game)

add_executable(path_to_home)
target_sources(path_to_home
    PRIVATE
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/Game.cpp
        ${SRC_DIR}/Meteor.cpp
        ${SRC_DIR}/Fuel.cpp
        ${SRC_DIR}/worklibgamer.cpp
        ${SRC_DIR}/Game.h
        ${SRC_DIR}/GameState.h
        ${SRC_DIR}/Meteor.h
        ${SRC_DIR}/Fuel.h
        ${SRC_DIR}/worklibgamer.h
)
target_include_directories(path_to_home PRIVATE ${SRC_DIR})
target_compile_features(path_to_home PRIVATE cxx_std_20)

set(SFML_REQUIRED_COMPONENTS system window graphics audio)
set(SFML_MIN_VERSION 2.5)

option(USE_SYSTEM_SFML "Use system-installed SFML instead of FetchContent" ON)

if(USE_SYSTEM_SFML)
    find_package(SFML ${SFML_MIN_VERSION} QUIET COMPONENTS ${SFML_REQUIRED_COMPONENTS})
endif()

if(NOT SFML_FOUND)
    include(FetchContent)
    set(SFML_VERSION 2.6.1)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG ${SFML_VERSION}
        GIT_SHALLOW TRUE
    )

    set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
    set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
    set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SFML)
endif()

target_link_libraries(path_to_home PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

set(ASSET_DIRS Font Image Music)
foreach(dir ${ASSET_DIRS})
    add_custom_command(TARGET path_to_home POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SRC_DIR}/${dir}
        $<TARGET_FILE_DIR:path_to_home>/${dir})
endforeach()
