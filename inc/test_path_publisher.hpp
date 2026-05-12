#ifndef TEST_PATH_PUBLISHER_HPP
#define TEST_PATH_PUBLISHER_HPP

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>

/**
 * @brief Test node for publishing sample path data
 * 
 * Publishes a sample square path on /path topic for testing
 * visualization without requiring actual SLAM node running.
 */
class TestPathPublisher : public rclcpp::Node {
public:
    TestPathPublisher();

private:
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int path_index_;
    
    void publishPath();
};

#endif // TEST_PATH_PUBLISHER_HPP
