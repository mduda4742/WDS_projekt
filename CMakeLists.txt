cmake_minimum_required(VERSION 3.16)

project(LeoDash VERSION 1.0 LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatic generation of moc, uic, and rcc files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 libraries
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGL OpenGLWidgets)

# Find ROS 2 libraries
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/window.cpp
    src/slam_page.cpp
    src/imu_page.cpp
    src/ros_node.cpp 
    src/battery_widget.cpp
    src/control_pad_widget.cpp
    src/language_manager.cpp
    src/view_25d_widget.cpp
    src/gl_rover_widget.cpp
    inc/window.hpp
    inc/slam_page.hpp
    inc/imu_page.hpp
    inc/ros_node.hpp 
    inc/battery_widget.hpp
    inc/control_pad_widget.hpp
    inc/gl_rover_widget.hpp
    inc/odomState.hpp
    inc/language_manager.hpp
    inc/view_25d_widget.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES})


# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    OpenGL::GL
    rclcpp::rclcpp
    std_msgs::std_msgs__rosidl_typesupport_cpp
    geometry_msgs::geometry_msgs__rosidl_typesupport_cpp
    sensor_msgs::sensor_msgs__rosidl_typesupport_cpp
    nav_msgs::nav_msgs__rosidl_typesupport_cpp
    tf2::tf2
    tf2_ros::tf2_ros
    tf2_geometry_msgs::tf2_geometry_msgs
    ${OpenCV_LIBS}
    cv_bridge::cv_bridge
)

# Add include folder to header search paths
target_include_directories(${PROJECT_NAME} PRIVATE inc)
