#ifndef CONTROL_PAD_WIDGET_HPP
#define CONTROL_PAD_WIDGET_HPP

#include <QWidget>
#include <QTimer>


/**
 * @class ControlPadWidget
 * @brief A custom UI widget providing a button-based control pad to drive the robot.
 * 
 * This widget captures button interactions (e.g., pressed/released events) and
 * periodically emits velocity commands via signals to drive the robot.
 */
class ControlPadWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for ControlPadWidget.
     * @param parent - pointer to the parent widget (default is nullptr).
     */
    explicit ControlPadWidget(QWidget *parent = nullptr);

signals:
    /**
     * @brief Signal emitted periodically to request robot movement.
     * @param vx - linear velocity request along the X-axis [m/s].
     * @param vth - angular velocity request around the Z-axis (yaw) [rad/s].
     */
    void velocityRequested(double vx, double vth);

private slots:
    /**
     * @brief Handles button press events to update target velocities based on direction.
     */
    void onButtonPressed();

    /**
     * @brief Handles button release events to reset target velocities and stop the robot.
     */
    void onButtonReleased();

    /**
     * @brief Timer callback that periodically emits the velocityRequested signal.
     */
    void sendVelocityCommand();

private:
    QTimer *publishTimer = nullptr;    ///< Timer used to periodically trigger and throttle velocity command emissions.
    double current_vx = 0.0;           ///< Current active target linear velocity [m/s].
    double current_vth = 0.0;          ///< Current active target angular velocity [rad/s].

    const double linear_speed_ = 0.5;  ///< Constant baseline forward/backward speed multiplier [m/s].
    const double angular_speed_ = 1.0; ///< Constant baseline rotational speed multiplier [rad/s].
};

#endif 