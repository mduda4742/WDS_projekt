#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QImage>

#include "battery_widget.hpp"
#include "control_pad_widget.hpp"
#include "ros_node.hpp"
#include "odomState.hpp"

/**
 * @class ImuPage
 * @brief Widget class that displays real-time telemetry data.
 * * The ImuPage provides a graphical interface to show the data processed
 * by the RosNode. It receives updates via slots and formats them into
 * human-readable text labels.
 */
class ImuPage : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for ImuPage.
     * @param parent Pointer to the parent widget (default is nullptr).
     */
    explicit ImuPage(QWidget *parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ImuPage() = default;

    void setRosNode(RosNode *node);

public slots:
    /**
     * @brief Updates the UI labels with new odometry and velocity data.
     * @details This slot receives the processed odomState, converts the
     * values (like Yaw to degrees), and updates the text on the labels.
     * @param state The current state of the robot containing pose and twist.
     */
    void updateOdom(odomState state);

    /**
     * @brief Updates the battery status label.
     * @param voltage Current battery voltage in Volts [V].
     */
    void updateBattery(double voltage);

    void updateCameraImage(const QImage &img);

private:
    QLabel *xLabel;           /**< Label displaying the X-coordinate [m]. */
    QLabel *yLabel;           /**< Label displaying the Y-coordinate [m]. */
    QLabel *yawLabel;         /**< Label displaying the orientation (Yaw) [deg]. */

    QLabel *linearVelLabel;   /**< Label displaying forward linear velocity [m/s]. */
    QLabel *angularVelLabel;  /**< Label displaying rotational velocity [deg/s]. */

    QLabel *batteryLabel;     /**< Label displaying the averaged battery voltage. */

    QLabel *cameraLabel;      /**< Placeholder for future camera stream visualization. */

    BatteryWidget *batteryIcon;
    ControlPadWidget *controlPad;
    
    RosNode *ros_node_ = nullptr;
};

#endif
