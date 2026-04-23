#include <ros_node.hpp>

using std::placeholders::_1;

RosNode::RosNode() : rclcpp::Node("qt_ros_node") {
    // Test sub
    test_sub_ = this->create_subscription<std_msgs::msg::String>(
        "test_topic", 10, std::bind(&RosNode::testCallback, this, std::placeholders::_1));

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

    RCLCPP_INFO(this->get_logger(), "RosNode has been started and is listening to topics");
}

void RosNode::testCallback(const std_msgs::msg::String::SharedPtr msg) {
    // Emit signal with the received message
    emit testDataReceived(QString::fromStdString(msg->data));
}

void RosNode::yawCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg) {
    double yaw = msg->vector.z;

    emit yawReceived(yaw);
}

void RosNode::batteryCallback(const std_msgs::msg::Float32::SharedPtr msg) {
    double voltage = msg->data;

    emit batteryReceived(voltage);
}
