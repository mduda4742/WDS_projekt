#include <ros_node.hpp>

using std::placeholders::_1;

RosNode::RosNode() : rclcpp::Node("qt_ros_node") {
    // Test sub
    test_sub_ = this->create_subscription<std_msgs::msg::String>(
        "test_topic", 10, std::bind(&RosNode::testCallback, this, std::placeholders::_1));

    rpy_sub_ = this->create_subscription<geometry_msgs::msg::Vector3Stamped>(
        "imu/rpy",
        10,
        std::bind(&RosNode::rpyCallback, this, _1)
    );

    RCLCPP_INFO(this->get_logger(), "RosNode has been started and is listening to topics");
}

void RosNode::testCallback(const std_msgs::msg::String::SharedPtr msg) {
    // Emit signal with the received message
    emit testDataReceived(QString::fromStdString(msg->data));
}

void RosNode::rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg) {
    double yaw   = msg->vector.z;

    emit rpyReceived(yaw);
}
