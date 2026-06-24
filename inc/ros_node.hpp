#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <QImage>
#include <vector>

// ROS 2 message types
#include <nav_msgs/msg/odometry.hpp>
#include <std_msgs/msg/float32.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <nav_msgs/msg/path.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include "odomState.hpp"

/**
 * @class RosNode
 * @brief Main ROS 2 node class that integrates with Qt. This node subscribes to sensor topics
 * (odometry, LIDAR, camera, battery, and SLAM path) and emits Qt signals when new data is received.
 * It also publishes velocity commands to control the robot.
 * * Subscriptions:
 * - /odom: Robot odometry data (nav_msgs/msg/Odometry)
 * - /battery_voltage: Battery voltage status (std_msgs/msg/Float32)
 * - /camera/image_color/compressed: Camera image data (sensor_msgs/msg/CompressedImage)
 * - /scan: LIDAR laser scan data (sensor_msgs/msg/LaserScan)
 * - /plan: Global path from SLAM/navigation (nav_msgs/msg/Path)
 * * Publications:
 * - /cmd_vel: Robot velocity commands (geometry_msgs/msg/Twist)
 */

class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    /**
     * @brief Constructor for the RosNode class. Initializes the ROS node with a name 
     * "qt_ros_node" and sets up subscriptions to odom, battery, image, scan and path topics.
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
    * @param[in] odom - the processed state containing x, y, yaw, and velocities (linear x and angular z).
    */
    void odomReceived(odomState odom);

    /**
    * @brief Signal emitted when a new battery voltage reading is received.
    * @param[in] voltage - the battery voltage in Volts [V].
    */
    void batteryReceived(double voltage);

    /**
     * @brief Signal emitted when a new camera image is received and processed.
     * @param[in] image - the processed image as a QImage, ready for GUI rendering.
     */
    void imageReceived(const QImage &image);
    
    /**
     * @brief Signal emitted when LIDAR scan data is received.
     * @param[in] ranges - vector of distance measurements in meters
     * @param[in] angle_min - minimum angle of scan in radians
     * @param[in] angle_max - maximum angle of scan in radians
     * @param[in] angle_increment - angular resolution of scan in radians
     */
    void laserScanReceived(const std::vector<float> &ranges, 
                          float angle_min, float angle_max, float angle_increment);
    
    /**
     * @brief Signal emitted when SLAM path data is received.
     * @param[in] path_x - vector of X coordinates along the path
     * @param[in] path_y - vector of Y coordinates along the path
     */
    void pathReceived(const std::vector<double> &path_x, const std::vector<double> &path_y);
    
    /**
     * @brief Signal emitted when robot position is updated (from cmd_vel integration or odometry).
     * @param[in] x - robot X position in meters
     * @param[in] y - robot Y position in meters
     * @param[in] theta - robot orientation angle in radians
     */
    void robotPoseReceived(double x, double y, double theta);

public:
    /**
     * @brief Publish velocity command to cmd_vel topic for robot movement control.
     * Creates and publishes a Twist message with linear and angular velocities.
     * @param[in] linear_x - forward /backward velocity in m/s (positive = forward)
     * @param[in] angular_z - rotation velocity in rad/s (positive = counter-clockwise)
     */
    void publishVelocity(double linear_x, double angular_z);

private:
    /**
     * @brief Callback for the odometry subscription.
     * @param[in] msg - shared pointer to the incoming nav_msgs/Odometry message.
     */
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg);

    /**
     * @brief Callback for the battery voltage subscription.
     * @param[in] msg - shared pointer to the incoming std_msgs/Float32 message.
     */
    void batteryCallback(const std_msgs::msg::Float32::SharedPtr msg);

    /**
     * @brief Callback function for the camera image subscription.
     * Converts the ROS message to a QImage format and emits the imageReceived signal.
     * @param[in] msg - shared pointer to the incoming sensor_msgs/CompressedImage message.
     */
    void imageCallback(const sensor_msgs::msg::CompressedImage::SharedPtr msg);

    /**
     * @brief Callback function for /scan subscription.
     * Extracts laser scan data and emits laserScanReceived signal for visualization.
     * @param[in] msg - the LaserScan message containing distance measurements
     */
    void laserScanCallback(const sensor_msgs::msg::LaserScan::SharedPtr msg);
    
    /**
     * @brief Callback function for SLAM path subscription.
     * Extracts path poses and emits pathReceived signal for visualization.
     * @param[in] msg - the Path message containing a series of poses
     */
    void pathCallback(const nav_msgs::msg::Path::SharedPtr msg);
    
    /// Robot pose tracking for movement visualization
    double robot_x_ = 0.0;      ///< Robot X position (meters)
    double robot_y_ = 0.0;      ///< Robot Y position (meters)
    double robot_theta_ = 0.0;  ///< Robot orientation angle (radians)
    double last_update_time_ = 0.0;  ///< Last time pose was updated (seconds)

    /// Path accumulation for trajectory visualization
    std::vector<double> path_x_;      ///< Accumulated X coordinates of robot path
    std::vector<double> path_y_;      ///< Accumulated Y coordinates of robot path
    double last_path_emit_time_ = 0.0;  ///< Last time path was emitted (seconds)

    // ROS 2 Subscriptions
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;             ///< Subscription for robot odometry data
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr battery_sub_;           ///< Subscription for battery voltage readings
    rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr image_sub_;  ///< Subscription for compressed camera images
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub_;        ///< Subscription for LIDAR laser scan data
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr path_sub_;                 ///< Subscription for global SLAM path data

    // ROS 2 Publisher
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;           ///< Publisher for robot velocity commands
};

#endif
