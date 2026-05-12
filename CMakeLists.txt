cmake_minimum_required(VERSION 3.16)

project(WDS VERSION 1.0 LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatic generation of moc, uic, and rcc files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 libraries
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

# Find ROS 2 libraries
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/window.cpp
    src/slam_page.cpp
    src/imu_page.cpp
    src/ros_node.cpp
    src/lidar_sim_node.cpp  
    inc/window.hpp
    inc/slam_page.hpp
    inc/imu_page.hpp
    inc/ros_node.hpp
    inc/lidar_sim_node.hpp  
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link Qt and ROS libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    rclcpp::rclcpp
    std_msgs::std_msgs__rosidl_typesupport_cpp
    geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
    sensor_msgs::sensor_msgs__rosidl_typesupport_cpp
    nav_msgs::nav_msgs__rosidl_typesupport_cpp
)

# Add include folder to header search paths
target_include_directories(${PROJECT_NAME} PRIVATE inc)

# Create test executable for path and pose visualization testing
add_executable(test_path_pub src/test_path_publisher.cpp)

target_link_libraries(test_path_pub PRIVATE 
    rclcpp::rclcpp
    nav_msgs::nav_msgs__rosidl_typesupport_cpp
    geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
    tf2::tf2
    tf2_geometry_msgs::tf2_geometry_msgs
)

target_include_directories(test_path_pub PRIVATE inc)