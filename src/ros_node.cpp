#include <ros_node.hpp>
#include <cmath>

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

    yaw_sub_ = this->create_subscription<geometry_msgs::msg::Vector3Stamped>(
        "imu/rpy",
        10,
        std::bind(&RosNode::yawCallback, this, _1)
    );


    voltage_sub_ = this->create_subscription<std_msgs::msg::Float32>(
        "firmware/battery_averaged",
        rclcpp::SensorDataQoS(),
        std::bind(&RosNode::batteryCallback, this, _1)
        );

    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan",
        10,
        std::bind(&RosNode::laserScanCallback, this, _1)
    );

    // Subscribe to SLAM path data
    path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
        "/path",
        10,
        std::bind(&RosNode::pathCallback, this, _1)
    );

    // Create publisher for robot velocity commands
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "cmd_vel",
        10
    );

    last_update_time_ = this->now().seconds();

    RCLCPP_INFO(this->get_logger(), "RosNode has been started and is listening to topics");
}


void RosNode::yawCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg) {
    double yaw = msg->vector.z;
    robot_theta_ = yaw;
    emit yawReceived(yaw);
}

void RosNode::batteryCallback(const std_msgs::msg::Float32::SharedPtr msg) {
    double voltage = msg->data;

    emit batteryReceived(voltage);
}

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
 * @brief Callback for SLAM path data. Extracts poses and emits signal.
 * @param msg The Path message containing a series of poses
 */
void RosNode::pathCallback(const nav_msgs::msg::Path::SharedPtr msg) {
    std::vector<double> path_x;
    std::vector<double> path_y;
    
    for (const auto &pose_stamped : msg->poses) {
        path_x.push_back(pose_stamped.pose.position.x);
        path_y.push_back(pose_stamped.pose.position.y);
    }
    
    emit pathReceived(path_x, path_y);

    // If path is not empty, extract the last pose as the robot's current pose
    if (!msg->poses.empty()) {
        const auto& last_pose = msg->poses.back().pose;
        robot_x_ = last_pose.position.x;
        robot_y_ = last_pose.position.y;
        
        // Convert quaternion to yaw for orientation
        auto q = last_pose.orientation;
        robot_theta_ = std::atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z));

        emit robotPoseReceived(robot_x_, robot_y_, robot_theta_);
    }
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
    
    cmd_vel_pub_->publish(twist_msg);
}