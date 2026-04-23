#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>
#include <std_msgs/msg/float32.hpp>
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
   /**
    * @brief Signal emitted when new data is received from the ROS topic. The message content 
    * is passed as a QString to be easily used in the Qt GUI.
    * @param msg The message content received from the ROS topic, converted to QString format.
    */
    void testDataReceived(const QString &msg); // testing

    void yawReceived(double yaw);

    void batteryReceived(double voltage);
    void rpyReceived(double yaw);
    void laserScanReceived(const std::vector<float> &ranges, 
                          float angle_min, float angle_max, float angle_increment);

private:
    void rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg);
    void laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg);

    /**
     * @brief ROS 2 subscription for the test topic. This subscription listens for messages on the "test_topic" 
     * and triggers the testCallback function when new messages arrive. This is used for testing the integration between ROS and Qt.
     */
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr test_sub_; // testing

    void yawCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg);
    rclcpp::Subscription<geometry_msgs::msg::Vector3Stamped>::SharedPtr yaw_sub_;

    void batteryCallback(const std_msgs::msg::Float32::SharedPtr msg);
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr voltage_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Vector3Stamped>::SharedPtr rpy_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;
};

#endif 
