# Path and Robot Position Testing Guide

## Overview
This guide explains how to test the path visualization and robot position tracking features using a dedicated test publisher node.

## Components

### 1. Test Path Publisher Node (`test_path_pub`)
- **Purpose**: Publishes sample path and pose data for visualization testing
- **Topic**: `/path` (nav_msgs/Path)
- **Behavior**: Publishes a square path pattern with 10 waypoints
  - Start: (0, 0)
  - Square vertices: (5, 0) → (5, 5) → (0, 5) → (0, 0)
  - Publishes every 500ms with interpolated positions

### 2. Main Application
- **Executable**: `WDS`
- **Purpose**: Visualizes path and robot position on SLAM page
- **Displays**: 
  - Yellow lines connecting path waypoints
  - White dots at waypoint locations
  - Robot position marker based on latest path pose

## Build Instructions

### 1. Build the project
```bash
cd /home/radek/Pulpit/Studia/sem_6/WDS/WDS_projekt
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

### 2. Verify build succeeded
```bash
ls -la WDS
ls -la test_path_pub
```

## Testing Steps

### Terminal 1: Start ROS 2 Daemon
```bash
source /opt/ros/jazzy/setup.bash
ros2 daemon start
```

### Terminal 2: Run Test Path Publisher
```bash
cd /home/radek/Pulpit/Studia/sem_6/WDS/WDS_projekt/build
source /opt/ros/jazzy/setup.bash
./test_path_pub
```

Expected output:
```
[INFO] Test Path Publisher started
[INFO] Publishing sample path on /path topic
```

### Terminal 3: Run Main Application
```bash
cd /home/radek/Pulpit/Studia/sem_6/WDS/WDS_projekt/build
source /opt/ros/jazzy/setup.bash
./WDS
```

### What to Look For:

1. **Path Visualization**:
   - Yellow lines should appear on the SLAM page connecting waypoints
   - White dots should mark waypoint locations
   - Path should form a square pattern

2. **Robot Position**:
   - Green circle should update position as path is published
   - Robot position should be at the latest waypoint
   - Orientation (theta) should point toward next waypoint

3. **LIDAR Display** (if LIDAR node also running):
   - Cyan dots showing LIDAR point cloud
   - Should be visible within 25m radius

## Monitoring with ROS 2 CLI

### Check what topics are published
```bash
ros2 topic list
```

Expected topics:
- `/path` - Path from test publisher
- `/scan` - LIDAR data (if lidar sim running)
- `/imu/rpy` - IMU data (if available)
- `/cmd_vel` - Velocity commands

### Monitor path data in real-time
```bash
ros2 topic echo /path --truncate-length 100
```

This shows:
- Number of poses in path
- X, Y coordinates of each waypoint
- Orientation quaternion for each pose

### Check robot position updates
You can see position changes by observing the timestamp changes:
```bash
ros2 topic echo /path/poses[0]
```

## Troubleshooting

### Path not showing in visualization
1. **Check connection**: Verify RosNode is receiving path messages
   - Add debug logging to pathCallback in ros_node.cpp
   - Check if pathReceived signal is being emitted

2. **Check signal connection**: Verify in main.cpp:
   ```cpp
   QObject::connect(ros_node.get(), &RosNode::pathReceived,
                    window.get(), &Window::updatePathData);
   ```

3. **Check topic name**: Both publisher and subscriber must use `/path`:
   - Test publisher uses: `/path`
   - RosNode subscribes to: `/path`

### Robot marker not updating
1. **Check robot pose extraction**: In pathCallback, ensure last pose is being extracted
2. **Check robotPoseReceived signal**: Should be emitted with x, y, theta
3. **Check visualization**: MapWidget::drawRobotMarker() needs implementation

### Performance Issues
1. Reduce path update frequency in test_path_pub (currently 500ms)
2. Adjust LIDAR radius if visualization is too crowded
3. Check CPU usage: `top` or `htop`

## Advanced Testing

### Test with varying path patterns
Edit `test_path_publisher.cpp` to change square_points:

```cpp
std::vector<std::pair<double, double>> square_points = {
    {0.0, 0.0},
    {2.0, 0.0},
    {4.0, 0.0},
    // Add more points here...
};
```

Then rebuild:
```bash
cd build && make -j$(nproc)
```

### Test with different time intervals
Modify timer interval in TestPathPublisher constructor:
```cpp
timer_ = this->create_wall_timer(
    std::chrono::milliseconds(250),  // Change from 500 to test faster/slower
    std::bind(&TestPathPublisher::publishPath, this)
);
```

### Record and replay test data
```bash
# Record path messages
ros2 bag record /path -o test_path_recording

# Later, replay the recording
ros2 bag play test_path_recording
```

## Expected Behavior Timeline

1. **0s**: Application starts, SLAM page shows empty 25m × 25m grid
2. **+1s**: Test publisher starts, path appears as yellow lines and white dots
3. **+2s**: Robot position marker (green circle) appears at first waypoint
4. **+5-10s**: Robot position smoothly interpolates between waypoints as path updates

## Integration with Real SLAM

To use real SLAM toolbox output instead of test data:

1. Run real robot or simulation with SLAM node
2. SLAM node publishes to `/path` topic
3. Ensure `/path` messages use `map` frame
4. No code changes needed - application auto-detects and visualizes

## Cleanup

### Stop processes
```bash
# Ctrl+C in each terminal to stop processes
```

### Clean build (if needed)
```bash
cd /home/radek/Pulpit/Studia/sem_6/WDS/WDS_projekt/build
rm -rf * && cmake .. && make -j$(nproc)
```
