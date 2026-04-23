# AI Coding Agent Instructions for WDS_projekt - LIDAR Visualization Edition

## Project Overview
This project visualizes sensor data from a Leo Rover using Qt6 GUI and ROS 2 integration, with a focus on LIDAR data visualization.

### Key Components

#### UI Architecture
- **Window**: Main application container managing tabs
- **SlamPage**: LIDAR visualization with MapWidget
- **MapWidget**: Custom QWidget that renders LIDAR scan data
- **ImuPage**: IMU sensor display

#### ROS Integration
- **RosNode**: Subscribes to `/scan` (LaserScan) and `/imu/rpy` (Vector3Stamped)
- **lidar_sim_node**: External node that publishes synthetic laser scan data

## LIDAR Visualization System

### Signal/Slot Flow
```
lidar_sim_node (publishes LaserScan)
    ↓ /scan topic
RosNode (subscribes, emits laserScanReceived signal)
    ↓ signal with ranges + angle data
Window (relay to appropriate page)
    ↓ updateLaserData slot
SlamPage (relay to MapWidget)
    ↓ updateLaserData slot
MapWidget (renders visualization)
    ↓ paintEvent() draws points and connections
```

### MapWidget Rendering
The MapWidget renders LIDAR data on a 2D plane:
- **Background**: Black (environment space)
- **Grid**: Dark gray lines (50 pixel spacing = reference grid)
- **Robot**: Green circle at center (robot position)
- **LIDAR Points**: Cyan dots (individual measurements)
- **Connections**: Red lines connecting consecutive points

**Coordinate System**:
- Center = robot position (0, 0)
- X-axis = forward direction
- Y-axis = left direction
- Scale: Adjusts dynamically; default ~50 pixels/meter

### Data Flow in MapWidget
1. Receives vector of ranges (distance measurements)
2. Receives angle_min, angle_max, angle_increment (from LaserScan header)
3. Converts polar coordinates (angle, range) → Cartesian (x, y)
4. Scales and translates to screen coordinates
5. Draws lines and points in paintEvent()

## File Organization

```
inc/
  ros_node.hpp           - ROS 2 integration, subscribes to topics
  slam_page.hpp          - SLAM visualization page
  window.hpp             - Main window, acts as signal hub
src/
  ros_node.cpp
  slam_page.cpp          - Contains MapWidget implementation
  window.cpp
  main.cpp               - Creates and spins RosNode
CMakeLists.txt          - Build with sensor_msgs dependency
```

**External Node:**
- `lidar_sim_node` - Publishes to `/scan` topic (run separately)

## Developer Workflows

### Building
```bash
cd build
cmake ..
make
```

### Running
```bash
source /opt/ros/<distro>/setup.bash
./WDS
```

The app will automatically start receiving LIDAR data from the LidarSimulator node.

### Testing Real LIDAR
1. Run a real LIDAR node publishing to `/scan` topic
2. Application will receive and visualize the data automatically
3. Disable LidarSimulator if real hardware is available

## Modifying LIDAR Rendering

### Changing Visualization Scale
In `slam_page.cpp`, `MapWidget::drawLaserScan()`:
```cpp
float maxRange = 10.0f;  // meters - adjust this
float scale = std::min(width(), height()) / (2.0f * maxRange);
```

### Changing Point/Line Colors
In `slam_page.cpp`:
```cpp
painter.setPen(QPen(Qt::red, 2));     // Lines color and width
painter.setPen(QPen(Qt::cyan, 3));    // Points color and size
```

### Changing Grid Appearance
In `MapWidget::paintEvent()`:
```cpp
int gridSpacing = 50;  // pixels
painter.setPen(QPen(Qt::darkGray, 1));  // grid color and width
```

### Filtering Bad Data
In `MapWidget::drawLaserScan()`, the code already filters:
```cpp
if (!std::isfinite(range) || range <= 0.0f) {
    continue;  // Skip invalid measurements
}
```

## Adding Custom Processing

### Customizing LIDAR Simulator
Edit `inc/lidar_simulator.hpp`:
- Modify `publishScan()` to generate different obstacle patterns
- Adjust angle resolution by changing `angle_increment`
- Modify noise model with different `std::distributions`
- Change publish rate by adjusting timer period

### Adding Obstacle Detection
In `slam_page.cpp`, add processing before rendering:
```cpp
void MapWidget::updateLaserScan(...) {
    // Process data: detect obstacles, cluster points, etc.
    ranges_ = processRanges(ranges);
    // Then render
    update();
}
```

## Architecture Benefits

✅ **Modular**: Each component handles one responsibility  
✅ **Extensible**: Easy to add new visualization modes or processing  
✅ **Testable**: LidarSimulator generates reproducible data  
✅ **Thread-safe**: Qt signals ensure UI updates on correct thread  
✅ **Decoupled**: Pages don't depend on ROS details  

## Integration Points

### Qt Requirements
- Qt6::Widgets (QWidget, QPainter, QMainWindow)
- CMAKE_AUTOMOC for Q_OBJECT support
- QPainter for 2D rendering

### ROS 2 Requirements
- sensor_msgs::msg::LaserScan message type
- rclcpp::Node for subscription
- rclcpp::executors::SingleThreadedExecutor for multi-node spinning

### Thread Safety
- ROS 2 executor runs in separate thread
- All UI updates via Qt signals/slots (thread-safe)
- MapWidget re-renders via `update()` which queues paintEvent() on UI thread

---

For LIDAR-specific questions, check MapWidget implementation in `slam_page.cpp`.
