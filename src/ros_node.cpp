#include <ros_node.hpp>

using std::placeholders::_1;

RosNode::RosNode() : rclcpp::Node("qt_ros_node") {

    rpy_sub_ = this->create_subscription<geometry_msgs::msg::Vector3Stamped>(
        "imu/rpy",
        10,
        std::bind(&RosNode::rpyCallback, this, _1)
    );

    laser_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan",
        10,
        std::bind(&RosNode::laserScanCallback, this, _1)
    );

    RCLCPP_INFO(this->get_logger(), "RosNode has been started and is listening to topics");
}

void RosNode::rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg) {
    double yaw = msg->vector.z;
    emit rpyReceived(yaw);
}

void RosNode::laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    std::vector<float> ranges(msg->ranges.begin(), msg->ranges.end());
    emit laserScanReceived(ranges, msg->angle_min, msg->angle_max, msg->angle_increment);
}