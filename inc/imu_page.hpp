#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "battery_widget.hpp"
#include "control_pad_widget.hpp"
#include "ros_node.hpp"

/**
 * @class ImuPage
 * @brief IMU (Inertial Measurement Unit) data and system status display page.
 * 
 * Shows battery voltage, robot pose (X, Y, Yaw), and movement velocities.
 * Updates in real-time as data is received from the robot.
 */
class ImuPage : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for ImuPage. Creates layout with status information display.
     * @param parent Parent widget
     */
    explicit ImuPage(QWidget *parent = nullptr);
    /**
     * @brief Destructor for ImuPage.
     */
    virtual ~ImuPage() = default;

    void setRosNode(RosNode *node);

public slots:

    /*
    void updateAccelData(const QString &msg);
    /**
     * @brief Update gyro (rotational velocity) data display.
     * @param msg Formatted gyroscope data string
     */
    void updateGyroData(const QString &msg);
    

    void updateYaw(double yaw);
    void updateVoltage(double voltage);

private:
    QLabel *accelLabel;
    QLabel *gyroLabel;

    QLabel *cameraLabel;
    QLabel *batteryLabel;

    BatteryWidget *batteryIcon;
    ControlPadWidget *controlPad;
    
    RosNode *ros_node_ = nullptr;

    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *yawLabel;

    QLabel *linearVelLabel;
    QLabel *angularVelLabel;
};

#endif
