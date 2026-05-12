#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#include <chrono>
#include <cmath>

/**
 * @brief Test node that publishes sample path and robot pose data for testing visualization.
 * 
 * This node publishes:
 * - A sample path on /path topic with waypoints forming a square pattern
 * - Updates simulate robot movement along the path
 */
class TestPathPublisher : public rclcpp::Node {
public:
    TestPathPublisher() : rclcpp::Node("test_path_publisher") {
        // Create publisher for /path topic
        path_pub_ = this->create_publisher<nav_msgs::msg::Path>("/path", 10);
        
        // Create a timer to periodically publish path updates
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(500),
            std::bind(&TestPathPublisher::publishPath, this)
        );
        
        RCLCPP_INFO(this->get_logger(), "Test Path Publisher started");
        RCLCPP_INFO(this->get_logger(), "Publishing sample path on /path topic");
    }

private:
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    int path_index_ = 0;
    
    /**
     * @brief Publish a sample square path with waypoints
     * 
     * Creates a path that traces a square pattern:
     * Starting at (0, 0) -> (5, 0) -> (5, 5) -> (0, 5) -> (0, 0)
     * and repeats, allowing visualization of continuous path following
     */
    void publishPath() {
        auto path_msg = nav_msgs::msg::Path();
        path_msg.header.frame_id = "map";
        path_msg.header.stamp = this->now();
        
        // Define square waypoints (in meters)
        std::vector<std::pair<double, double>> square_points = {
            {0.0, 0.0},
            {2.0, 0.0},
            {4.0, 0.0},
            {5.0, 1.0},
            {5.0, 3.0},
            {5.0, 5.0},
            {3.0, 5.0},
            {1.0, 5.0},
            {0.0, 4.0},
            {0.0, 2.0},
            {0.0, 3.0},
            {0.0, 4.0},
            {0.0, 5.0},
            {0.0, 6.0},
            {0.0, 10.0},

        };
        
        // Build path with all waypoints
        for (size_t i = 0; i < square_points.size(); ++i) {
            auto pose_stamped = geometry_msgs::msg::PoseStamped();
            pose_stamped.header.frame_id = "map";
            pose_stamped.header.stamp = this->now();
            
            // Set position
            pose_stamped.pose.position.x = square_points[i].first;
            pose_stamped.pose.position.y = square_points[i].second;
            pose_stamped.pose.position.z = 0.0;
            
            // Calculate quaternion from angle (looking towards next point)
            double angle = 0.0;
            if (i < square_points.size() - 1) {
                double dx = square_points[i + 1].first - square_points[i].first;
                double dy = square_points[i + 1].second - square_points[i].second;
                angle = std::atan2(dy, dx);
            } else {
                double dx = square_points[0].first - square_points[i].first;
                double dy = square_points[0].second - square_points[i].second;
                angle = std::atan2(dy, dx);
            }
            
            tf2::Quaternion q;
            q.setRPY(0, 0, angle);
            pose_stamped.pose.orientation = tf2::toMsg(q);
            
            path_msg.poses.push_back(pose_stamped);
        }
        
        // Add some intermediate points to show smooth path
        // This adds points partway along the path to simulate continuous movement
        path_index_ = (path_index_ + 1) % (square_points.size() * 2);
        double interpolation = static_cast<double>(path_index_ % 2) * 0.5;
        
        if (path_index_ / 2 < static_cast<int>(square_points.size()) - 1) {
            size_t current_segment = path_index_ / 2;
            double x = square_points[current_segment].first + 
                      (square_points[current_segment + 1].first - square_points[current_segment].first) * interpolation;
            double y = square_points[current_segment].second + 
                      (square_points[current_segment + 1].second - square_points[current_segment].second) * interpolation;
            
            auto current_pose = geometry_msgs::msg::PoseStamped();
            current_pose.header.frame_id = "map";
            current_pose.header.stamp = this->now();
            current_pose.pose.position.x = x;
            current_pose.pose.position.y = y;
            current_pose.pose.position.z = 0.0;
            
            double dx = square_points[current_segment + 1].first - square_points[current_segment].first;
            double dy = square_points[current_segment + 1].second - square_points[current_segment].second;
            double angle = std::atan2(dy, dx);
            
            tf2::Quaternion q;
            q.setRPY(0, 0, angle);
            current_pose.pose.orientation = tf2::toMsg(q);
            
            path_msg.poses.push_back(current_pose);
        }
        
        path_pub_->publish(path_msg);
        
        RCLCPP_DEBUG(this->get_logger(), 
            "Published path with %ld waypoints", path_msg.poses.size());
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TestPathPublisher>();
    
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
