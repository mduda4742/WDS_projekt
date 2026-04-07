#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp> // testing

class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    RosNode();
    
signals:
    void testDataReceived(const QString &msg); // Signal to emit when a new message is received

private:
    void testCallback(const std_msgs::msg::String::SharedPtr msg); // testing
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr test_sub_; // testing
};

#endif 