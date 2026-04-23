#ifndef LIDAR_SIM_NODE_HPP
#define LIDAR_SIM_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

class LidarSimulator : public rclcpp::Node {
public:
    LidarSimulator();
    virtual ~LidarSimulator() = default;

private:
    void publishScan();

    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

#endif