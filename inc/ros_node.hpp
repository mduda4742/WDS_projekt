#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp>
#include <geometry_msgs/msg/vector3_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <std_msgs/msg/float32.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <nav_msgs/msg/path.hpp>
#include <QImage>
#include <vector>

// ROS 2 message types
#include <std_msgs/msg/float32.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <sensor_msgs/msg/image.hpp>

#include "odomState.hpp"

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
    * @brief Signal emitted when new odometry data is processed.
    * @param odom The processed state containing x, y, yaw, and velocities.
    */
    void odomReceived(odomState odom);

    /**
    * @brief Signal emitted when a new battery voltage reading is received.
    * @param voltage The battery voltage in Volts [V].
    */
    void batteryReceived(double voltage);

    void imageReceived(const QImage &image);
    
    /**
     * @brief Signal emitted when LIDAR scan data is received.
     * @param ranges Vector of distance measurements in meters
     * @param angle_min Minimum angle of scan in radians
     * @param angle_max Maximum angle of scan in radians
     * @param angle_increment Angular resolution of scan in radians
     */
    void laserScanReceived(const std::vector<float> &ranges, 
                          float angle_min, float angle_max, float angle_increment);
    
    /**
     * @brief Signal emitted when SLAM path data is received.
     * @param path_x Vector of X coordinates along the path
     * @param path_y Vector of Y coordinates along the path
     */
    void pathReceived(const std::vector<double> &path_x, const std::vector<double> &path_y);
    
    /**
     * @brief Signal emitted when robot position is updated (from cmd_vel integration or odometry).
     * @param x Robot X position in meters
     * @param y Robot Y position in meters
     * @param theta Robot orientation angle in radians
     */
    void robotPoseReceived(double x, double y, double theta);

public:
    /**
     * @brief Publish velocity command to cmd_vel topic for robot movement control.
     * Creates and publishes a Twist message with linear and angular velocities.
     * @param linear_x Forward/backward velocity in m/s (positive = forward)
     * @param angular_z Rotation velocity in rad/s (positive = counter-clockwise)
     */
    void publishVelocity(double linear_x, double angular_z);

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

    void imageCallback(const sensor_msgs::msg::Image::SharedPtr msg);

    /**
     * @brief Callback function for /scan subscription.
     * Extracts laser scan data and emits laserScanReceived signal for visualization.
     * @param msg The LaserScan message containing distance measurements
     */
    void laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg);
    
    /**
     * @brief Callback function for SLAM path subscription.
     * Extracts path poses and emits pathReceived signal for visualization.
     * @param msg The Path message containing a series of poses
     */
    void pathCallback(const nav_msgs::msg::Path::SharedPtr msg);
    
    /// Robot pose tracking for movement visualization
    double robot_x_ = 0.0;      ///< Robot X position (meters)
    double robot_y_ = 0.0;      ///< Robot Y position (meters)
    double robot_theta_ = 0.0;  ///< Robot orientation angle (radians)
    double last_update_time_ = 0.0;  ///< Last time pose was updated (seconds)

    // ROS 2 Subscriptions
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr battery_sub_;
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr path_sub_;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
};

#endif 
