#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <vector>


/**
 * @class RosNode
 * @brief Main ROS 2 node class that integrates with Qt. This node subscribes to topics
 * including LIDAR data and emits Qt signals when new data is received.
 */
class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    /**
     * @brief Constructor for the RosNode class. Initializes the ROS node with a name and sets up 
     * the subscriptions to various topics.
     */
    RosNode();
    /**
     * @brief Destructor for the RosNode class.
     */
    virtual ~RosNode() = default;
    
signals:
    void rpyReceived(double yaw);
    void laserScanReceived(const std::vector<float> &ranges, 
                          float angle_min, float angle_max, float angle_increment);

private:
    void rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg);
    void laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg);

    rclcpp::Subscription<geometry_msgs::msg::Vector3Stamped>::SharedPtr rpy_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;
};

#endif 
