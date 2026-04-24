#include <ros_node.hpp>
#include <tf2/utils.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

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
