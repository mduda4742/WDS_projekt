#include <ros_node.hpp>

using std::placeholders::_1;

/**
 * @brief RosNode constructor. Initializes ROS 2 node with subscriptions and publisher.
 * 
 * Sets up subscriptions to:
 * - /imu/rpy: IMU orientation data (yaw angle)
 * - /scan: LIDAR laser scan data
 * 
 * And creates publisher for:
 * - /cmd_vel: Robot velocity commands
 */
RosNode::RosNode() : rclcpp::Node("qt_ros_node") {

    // Subscribe to IMU RPY (roll, pitch, yaw) data
    rpy_sub_ = this->create_subscription<geometry_msgs::msg::Vector3Stamped>(
        "imu/rpy",
        10,
        std::bind(&RosNode::rpyCallback, this, _1)
    );

    // Subscribe to LIDAR laser scan data
    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan",
        10,
        std::bind(&RosNode::laserScanCallback, this, _1)
    );

    // Create publisher for robot velocity commands
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "cmd_vel",
        10
    );

    RCLCPP_INFO(this->get_logger(), "RosNode has been started and is listening to topics");
}

/**
 * @brief Callback for IMU RPY data. Extracts yaw and emits signal.
 * @param msg The Vector3Stamped message containing roll, pitch, yaw
 */
void RosNode::rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg) {
    // Extract yaw angle from the message and emit signal
    double yaw = msg->vector.z;
    emit rpyReceived(yaw);
}

/**
 * @brief Callback for LIDAR scan data. Extracts ranges and emits signal.
 * @param msg The LaserScan message containing distance measurements
 */
void RosNode::laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    // Convert LIDAR ranges to vector and emit signal with scan parameters
    std::vector<float> ranges(msg->ranges.begin(), msg->ranges.end());
    emit laserScanReceived(ranges, msg->angle_min, msg->angle_max, msg->angle_increment);
}

/**
 * @brief Publish velocity command to control robot movement.
 * 
 * @param linear_x Forward/backward velocity in m/s (positive = forward)
 * @param linear_y Strafe (left/right) velocity in m/s (positive = left)
 * @param angular_z Rotation velocity in rad/s (positive = counter-clockwise)
 */
void RosNode::publishVelocity(double linear_x, double linear_y, double angular_z) {
    // Create Twist message with provided velocities
    auto twist_msg = geometry_msgs::msg::Twist();
    twist_msg.linear.x = linear_x;   // Forward/backward
    twist_msg.linear.y = linear_y;   // Left/right strafe
    twist_msg.linear.z = 0.0;        // No vertical movement
    twist_msg.angular.x = 0.0;       // No roll rotation
    twist_msg.angular.y = 0.0;       // No pitch rotation
    twist_msg.angular.z = angular_z; // Yaw (rotation)
    
    // Publish to cmd_vel topic
    cmd_vel_pub_->publish(twist_msg);
}