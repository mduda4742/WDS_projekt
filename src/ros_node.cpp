#include <ros_node.hpp>
#include <cmath>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <cv_bridge/cv_bridge.hpp> // Mostek między ROS a OpenCV
#include <opencv2/opencv.hpp>   

using std::placeholders::_1;


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
        rclcpp::SensorDataQoS(),
        std::bind(&RosNode::imageCallback, this, _1)
    );

    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan",
        rclcpp::SensorDataQoS(),
        std::bind(&RosNode::laserScanCallback, this, _1)
    );

    
    path_sub_ = this->create_subscription<nav_msgs::msg::Path>(
        "/path",
        10,
        std::bind(&RosNode::pathCallback, this, _1)
    );

    // Subscribe to merged odometry for robot pose and trajectory
    odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
        "merged_odom",
        10,
        std::bind(&RosNode::odomCallback, this, _1)
    );
    
    cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>(
        "cmd_vel",
        10
    );

    last_update_time_ = this->now().seconds();

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
    
    // Update robot pose for marker visualization
    robot_x_ = state.x;
    robot_y_ = state.y;
    robot_theta_ = state.yaw;
    emit robotPoseReceived(robot_x_, robot_y_, robot_theta_);
    
    // Accumulate pose into path history
    path_x_.push_back(robot_x_);
    path_y_.push_back(robot_y_);
    
    // Limit path history to last 1000 poses to avoid memory issues
    const size_t max_path_size = 1000;
    if (path_x_.size() > max_path_size) {
        path_x_.erase(path_x_.begin());
        path_y_.erase(path_y_.begin());
    }
    
    // Emit path periodically (throttle to avoid excessive updates)
    double current_time = this->now().seconds();
    if (current_time - last_path_emit_time_ > 0.1) {  // Emit path every 100ms
        emit pathReceived(path_x_, path_y_);
        last_path_emit_time_ = current_time;
    }
}

void RosNode::batteryCallback(const std_msgs::msg::Float32::SharedPtr msg) {
    double voltage = msg->data;

    emit batteryReceived(voltage);
}

void RosNode::imageCallback(const sensor_msgs::msg::CompressedImage::SharedPtr msg) {
    if (msg->data.empty()) return;

    try {
        cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy(msg, "rgb8");

        QImage q_img(
            cv_ptr->image.data, 
            cv_ptr->image.cols, 
            cv_ptr->image.rows, 
            static_cast<int>(cv_ptr->image.step), 
            QImage::Format_RGB888
        );

        emit imageReceived(q_img.copy());

    } catch (const std::exception& e) {
        RCLCPP_ERROR(this->get_logger(), "Image callback error: %s", e.what());
    }
}

void RosNode::laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    // Convert LIDAR ranges to vector and emit signal with scan parameters
    std::vector<float> ranges(msg->ranges.begin(), msg->ranges.end());
    emit laserScanReceived(ranges, msg->angle_min, msg->angle_max, msg->angle_increment);
}

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

void RosNode::publishVelocity(double linear_x, double angular_z) {
    // Create Twist message with provided velocities
    auto twist_msg = geometry_msgs::msg::Twist();
    twist_msg.linear.x = linear_x;   // Forward/backward
    twist_msg.linear.y = 0.0;   // Left/right strafe
    twist_msg.linear.z = 0.0;        // No vertical movement
    twist_msg.angular.x = 0.0;       // No roll rotation
    twist_msg.angular.y = 0.0;       // No pitch rotation
    twist_msg.angular.z = angular_z; // Yaw (rotation)
    
    cmd_vel_pub_->publish(twist_msg);
}