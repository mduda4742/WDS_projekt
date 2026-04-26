#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/float32.hpp>

#include "odomState.hpp"

/**
 * @class RosNode
 * @brief Main ROS 2 node class that integrates Leo Rover data with the Qt GUI.
 * * This node subscribes to various topics, processes the incoming data,
 * and emits Qt signals to be consumed by the GUI thread.
 */
class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    /**
     * @brief Constructor for the RosNode class. Initializes the ROS node with a name and sets up 
     * the subscription to the test topic. 
     */
    RosNode();
    /**
     * @brief Destructor for the RosNode class. Currently defaulted, but can be used to clean up any resources if needed in the future.
     */
    virtual ~RosNode() = default;
    
signals:

    /**
    * @brief Signal emitted when new odometry data is processed.
    * @param odom The processed state containing x, y, yaw, and velocities.
    */
    void odomReceived(odomState odom);

    /**
    * @brief Signal emitted when a new battery voltage reading is received.
    * @param voltage The battery voltage in Volts [V].
    */
    void batteryReceived(double voltage);

private:

    /**
     * @brief Callback for the odometry subscription.
     * @param msg Shared pointer to the incoming nav_msgs/Odometry message.
     */
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg);

    /**
     * @brief Callback for the battery voltage subscription.
     * @param msg Shared pointer to the incoming std_msgs/Float32 message.
     */
    void batteryCallback(const std_msgs::msg::Float32::SharedPtr msg);

    // ROS 2 Subscriptions
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr battery_sub_;
};

#endif 
