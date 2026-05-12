#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>
#include <vector>

/**
 * @class PathTesterNode
 * @brief A ROS 2 node to publish a sample nav_msgs::msg::Path for testing.
 * 
 * This node publishes a square path segment by segment to the /path topic,
 * allowing for visualization and testing of path following and robot pose
 * updates in a client application.
 */
class PathTesterNode : public rclcpp::Node {
public:
    PathTesterNode() : Node("path_tester_node"), current_segment_(0) {
        publisher_ = this->create_publisher<nav_msgs::msg::Path>("/path", 10);
        
        // Define the points of the square path
        path_points_ = {
            {0.0, 0.0}, {2.0, 0.0}, {2.0, 2.0}, {0.0, 2.0}, {0.0, 0.0}
        };

        timer_ = this->create_wall_timer(
            std::chrono::seconds(3),
            std::bind(&PathTesterNode::publish_path_segment, this));
            
        RCLCPP_INFO(this->get_logger(), "Path tester node started. Publishing path segments every 3 seconds.");
    }

private:
    void publish_path_segment() {
        if (current_segment_ >= path_points_.size()) {
            RCLCPP_INFO(this->get_logger(), "Full path published. Resetting.");
            current_segment_ = 0;
        }

        auto path_msg = std::make_unique<nav_msgs::msg::Path>();
        path_msg->header.stamp = this->now();
        path_msg->header.frame_id = "map";

        // Add segments up to the current one
        for (size_t i = 0; i <= current_segment_ && i < path_points_.size(); ++i) {
            geometry_msgs::msg::PoseStamped pose;
            pose.header.stamp = this->now();
            pose.header.frame_id = "map";
            pose.pose.position.x = path_points_[i].first;
            pose.pose.position.y = path_points_[i].second;

            // Calculate orientation to point to the next segment
            double yaw = 0.0;
            if (i < path_points_.size() - 1) {
                yaw = std::atan2(path_points_[i+1].second - pose.pose.position.y, path_points_[i+1].first - pose.pose.position.x);
            } else if (i > 0) { // For the last point, keep previous orientation
                 yaw = std::atan2(pose.pose.position.y - path_points_[i-1].second, pose.pose.position.x - path_points_[i-1].first);
            }

            tf2::Quaternion q;
            q.setRPY(0, 0, yaw);
            pose.pose.orientation = tf2::toMsg(q);

            path_msg->poses.push_back(pose);
        }

        RCLCPP_INFO(this->get_logger(), "Publishing path with %zu poses. Current robot pose: (%.1f, %.1f)", 
            path_msg->poses.size(), path_msg->poses.back().pose.position.x, path_msg->poses.back().pose.position.y);
        publisher_->publish(std::move(*path_msg));

        current_segment_++;
    }

    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::vector<std::pair<double, double>> path_points_;
    size_t current_segment_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<PathTesterNode>());
    rclcpp::shutdown();
    return 0;
}