#include "lidar_sim_node.hpp"
#include <cmath>

LidarSimulator::LidarSimulator() : rclcpp::Node("lidar_simulator") {
    publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);
    timer_ = this->create_wall_timer(
            std::chrono::milliseconds(100),
            std::bind(&LidarSimulator::publishScan, this)
        );
        RCLCPP_INFO(this->get_logger(), "LidarSimulator node has been started");
}

void LidarSimulator::publishScan() {
    auto msg = sensor_msgs::msg::LaserScan();
    msg.header.stamp = this->now();
    msg.header.frame_id = "lidar_frame";
    msg.angle_min = -M_PI;
    msg.angle_max = M_PI;
    msg.angle_increment = M_PI / 180.0; // 1 degree resolution
    msg.range_min = 0.1;
    msg.range_max = 10.0;

    for (int i = 0; i < 360; ++i) {
        msg.ranges.push_back(2.0 + rand() % 10 / 100.0); // Simulate random distances around 2.0 meters with some noise
    }
    publisher_->publish(msg);
}