# LeoDash: Sensory Data Visualization

Sensory Data Visualization Project (formerly WDS)

## Overview
This project is a graphical user interface (GUI) application built with **C++**, **Qt6**, and **ROS 2**. It is designed to monitor and visualize real-time telemetry, 2D LIDAR scans, SLAM mapping, and camera feeds from a mobile robot or rover. It also features a custom 3D OpenGL viewport of the rover model and allows for manual remote control directly from the dashboard.

## Features
- **Real-time Telemetry:** Live display of robot odometry, position (X, Y, Yaw), and velocity (Linear, Angular).
- **LIDAR & SLAM Visualization:** 2D point cloud rendering and path tracking on a custom map widget.
- **Camera Integration:** Real-time video feed display using OpenCV and `cv_bridge`.
- **3D Rover Visualization:** OpenGL-based 3D rendering of the rover's model.
- **Robot Control:** Built-in control pad widget to publish velocity commands (`cmd_vel`) to the rover.
- **Battery Status:** Visual battery charge level indicator.
- **Multilingual Support:** Dynamic language switching between English and Polish.

## Screenshots

*Add your project screenshots here by replacing the placeholder image paths with the actual paths to your photos.*

### Home Page
!Home Screen Placeholder
> *The main welcome screen featuring the language selection and application introduction.*

### SLAM View
!SLAM View Placeholder
> *Real-time LIDAR point cloud visualization, SLAM path tracking, and movement controls.*

### IMU & Telemetry View
!IMU View Placeholder
> *Live display of odometry data, battery level, camera feed, and the 2.5D/3D visualization view.*

## Prerequisites
Ensure you have the following dependencies installed on your system before building the project:
- **CMake** (>= 3.16)
- A **C++17** compatible compiler
- **Qt6** (Widgets, Core, Gui, OpenGL, OpenGLWidgets)
- **ROS 2** (e.g., Humble, Iron, or newer) including the following packages:
  - `rclcpp`
  - `std_msgs`, `geometry_msgs`, `sensor_msgs`, `nav_msgs`
  - `tf2`, `tf2_ros`, `tf2_geometry_msgs`
- **OpenCV** and **cv_bridge**

## Building the Project

Ensure your ROS 2 environment is sourced before configuring CMake. 

```bash
# Source ROS 2 environment
source /opt/ros/$ROS_DISTRO$/setup.bash

# Create a build directory and compile
mkdir build
cd build
cmake ..
make
```

## Running the Application
After building successfully, you can start the application by running:
```bash
./LeoDash
```

*Note: Make sure your underlying ROS 2 network (LIDAR nodes, odometry broadcasters, SLAM, camera nodes) is running for the application to display real-time data.*
