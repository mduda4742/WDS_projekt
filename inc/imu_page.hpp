#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

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

public slots:
    /**
     * @brief Update accel (acceleration) data display.
     * @param msg Formatted acceleration data string
     */
    void updateAccelData(const QString &msg);
    /**
     * @brief Update gyro (rotational velocity) data display.
     * @param msg Formatted gyroscope data string
     */
    void updateGyroData(const QString &msg);

private:
    QLabel *accelLabel;  ///< Label for acceleration data
    QLabel *gyroLabel;   ///< Label for gyroscope data
};

#endif
