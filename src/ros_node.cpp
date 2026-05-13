#include <ros_node.hpp>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

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

    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
        "merged_odom",
        10,
        std::bind(&RosNode::odomCallback, this, _1)
    );

    battery_sub_ = this->create_subscription<std_msgs::msg::Float32>(
        "firmware/battery_averaged",
        rclcpp::SensorDataQoS(),
        std::bind(&RosNode::batteryCallback, this, _1)
    );


    image_sub_ = this->create_subscription<sensor_msgs::msg::CompressedImage>(
        "camera/image_color/compressed",
        10,
        std::bind(&RosNode::imageCallback, this, _1)
    );

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

void RosNode::odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg) {
    odomState state;
    state.x = msg->pose.pose.position.x;
    state.y = msg->pose.pose.position.y;

    state.yaw = tf2::getYaw(msg->pose.pose.orientation);

    state.linear_vel = msg->twist.twist.linear.x;
    state.angular_vel = msg->twist.twist.angular.z;

    emit odomReceived(state);
}

void RosNode::batteryCallback(const std_msgs::msg::Float32::SharedPtr msg) {
    double voltage = msg->data;

    emit batteryReceived(voltage);
}

void RosNode::imageCallback(const sensor_msgs::msg::CompressedImage::SharedPtr msg) {
    QImage image;
    
    if (image.loadFromData(msg->data.data(), static_cast<int>(msg->data.size()))) {
        emit imageReceived(image);
    } else {
        RCLCPP_ERROR(this->get_logger(), "Nie udało się zdekodować obrazu!");
    }
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
void RosNode::publishVelocity(double linear_x, double angular_z) {
    // Create Twist message with provided velocities
    auto twist_msg = geometry_msgs::msg::Twist();
    twist_msg.linear.x = linear_x;   // Forward/backward
    twist_msg.linear.y = 0.0;   // Left/right strafe
    twist_msg.linear.z = 0.0;        // No vertical movement
    twist_msg.angular.x = 0.0;       // No roll rotation
    twist_msg.angular.y = 0.0;       // No pitch rotation
    twist_msg.angular.z = angular_z; // Yaw (rotation)
    
    // Publish to cmd_vel topic
    cmd_vel_pub_->publish(twist_msg);
}