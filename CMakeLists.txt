cmake_minimum_required(VERSION 3.24)
project(Ants)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES OFF CACHE BOOL "" FORCE) # or games

set(IMGUI_FOLDER "libs/imgui")

set(IMGUI_SOURCES
        ${IMGUI_FOLDER}/imgui.cpp
        ${IMGUI_FOLDER}/imgui_demo.cpp
        ${IMGUI_FOLDER}/imgui_draw.cpp
        ${IMGUI_FOLDER}/imgui_widgets.cpp
        ${IMGUI_FOLDER}/imgui_tables.cpp
        libs/rlImGui/rlImGui.cpp
        )

add_subdirectory(libs/raylib)
include_directories(${IMGUI_FOLDER})
include_directories(libs/rlImGui)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI_SOURCES} Ant.cpp Ant.hpp World.cpp World.hpp Simulation.cpp Simulation.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib -static gcc stdc++ winpthread -dynamic)