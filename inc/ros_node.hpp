#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float32.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <vector>


/**
 * @class RosNode
 * @brief Main ROS 2 node class that integrates with Qt. This node subscribes to topics
 * including LIDAR data and IMU rotation, and emits Qt signals when new data is received.
 * Also publishes velocity commands to control the robot.
 * 
 * Subscriptions:
 * - /imu/rpy: Robot orientation (yaw angle) as Vector3Stamped
 * - /scan: LIDAR laser scan data as LaserScan
 * 
 * Publications:
 * - /cmd_vel: Robot velocity commands as Twist messages
 */
class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    /**
     * @brief Constructor for the RosNode class. Initializes the ROS node with a name 
     * "qt_ros_node" and sets up subscriptions to /imu/rpy and /scan topics.
     * Also creates a publisher for /cmd_vel commands.
     */
    RosNode();
    /**
     * @brief Destructor for the RosNode class. Cleans up subscriptions and publishers.
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
    
    /**
     * @brief Signal emitted when LIDAR scan data is received.
     * @param ranges Vector of distance measurements in meters
     * @param angle_min Minimum angle of scan in radians
     * @param angle_max Maximum angle of scan in radians
     * @param angle_increment Angular resolution of scan in radians
     */
    void laserScanReceived(const std::vector<float> &ranges, 
                          float angle_min, float angle_max, float angle_increment);

public:
    /**
     * @brief Publish velocity command to cmd_vel topic for robot movement control.
     * Creates and publishes a Twist message with linear and angular velocities.
     * @param linear_x Forward/backward velocity in m/s (positive = forward)
     * @param linear_y Strafe (left/right) velocity in m/s (positive = left)
     * @param angular_z Rotation velocity in rad/s (positive = counter-clockwise)
     */
    void publishVelocity(double linear_x, double linear_y, double angular_z);

private:
    /**
     * @brief Callback function for /imu/rpy subscription.
     * Extracts yaw angle and emits rpyReceived signal.
     * @param msg The Vector3Stamped message containing roll, pitch, yaw
     */
    void rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr msg);
    
    /**
     * @brief Callback function for /scan subscription.
     * Extracts laser scan data and emits laserScanReceived signal for visualization.
     * @param msg The LaserScan message containing distance measurements
     */
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
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
};

#endif 
