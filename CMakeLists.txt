cmake_minimum_required(VERSION 3.16)

project(WDS VERSION 1.0 LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automaatic generation of moc, uic, and rcc files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 libraries
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Add source files
set(SOURCES
    src/main.cpp
    src/window.cpp
    inc/window.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
)

# Add include folder to header search paths
target_include_directories(${PROJECT_NAME} PRIVATE inc)