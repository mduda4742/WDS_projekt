#ifndef ODOMSTATE_HPP
#define ODOMSTATE_HPP

/**
 * @struct odomState
 * @brief Represents the processed pose and velocity of the robot.
 * * This structure stores the 2D position, orientation, and velocity
 * data extracted from the ROS 2 odometry topic.
 */
struct odomState {
    double x = 0.0;           /**< Position in X axis [m] */
    double y = 0.0;           /**< Position in Y axis [m] */
    double yaw = 0.0;         /**< Rotation around Z axis [rad] */
    double linear_vel = 0.0;  /**< Forward linear velocity [m/s] */
    double angular_vel = 0.0; /**< Rotational velocity [rad/s] */
};

#endif // ODOMSTATE_HPP
