#include <ros_node.hpp>

RosNode::RosNode() : rclcpp::Node("qt_ros_node") {
    // Test sub
    test_sub_ = this->create_subscription<std_msgs::msg::String>(
        "test_topic", 10, std::bind(&RosNode::testCallback, this, std::placeholders::_1));
}

void RosNode::testCallback(const std_msgs::msg::String::SharedPtr msg) {
    // Emit signal with the received message
    emit testDataReceived(QString::fromStdString(msg->data));
}