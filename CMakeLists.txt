cmake_minimum_required(VERSION 3.24)
project(Ants)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_GAMES OFF CACHE BOOL "" FORCE) # or games

set(SOURCE_FILES
        Ant.cpp
        Ant.hpp
        World.cpp
        World.hpp
        Simulation.cpp
        Simulation.hpp
        Settings.hpp
        Utils/ColorConvert.hpp
        Utils/IntVec.hpp
        Utils/Timer.hpp
        Utils/Random.hpp
        Brush.hpp
        Brush.cpp
        Settings.cpp
        Nest.cpp
        Nest.hpp
        PheromoneMap.cpp
        PheromoneMap.hpp
        TileMap.cpp
        TileMap.hpp
        Tile.cpp
        Tile.hpp
        Utils/BoundsChecker.hpp
        ColorMap.cpp
        ColorMap.hpp
        AntColony.cpp AntColony.hpp)

set(IMGUI_FOLDER "libs/imgui")

set(IMGUI_SOURCES
        ${IMGUI_FOLDER}/imgui.cpp
        ${IMGUI_FOLDER}/imgui_demo.cpp
        ${IMGUI_FOLDER}/imgui_draw.cpp
        ${IMGUI_FOLDER}/imgui_widgets.cpp
        ${IMGUI_FOLDER}/imgui_tables.cpp
        ${IMGUI_FOLDER}/misc/cpp/imgui_stdlib.cpp
        libs/rlImGui/rlImGui.cpp
        )

find_package(OpenMP)

add_subdirectory(libs/raylib)
include_directories(${IMGUI_FOLDER})
include_directories(libs/rlImGui)
include_directories(libs)
include_directories(Utils)

add_executable(${PROJECT_NAME} main.cpp ${IMGUI_SOURCES} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall ${OpenMP_CXX_FLAGS})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib -static gcc stdc++ winpthread -dynamic ${OpenMP_CXX_FLAGS})
