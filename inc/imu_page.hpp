#ifndef IMU_PAGE_HPP
#define IMU_PAGE_HPP

#include <QWidget>
#include <QLabel>
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
     * @param parent - pointer to the parent widget (default is nullptr).
     */
    explicit ImuPage(QWidget *parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ImuPage() = default;

    /**
     * @brief Sets the ROS node instance to enable interaction with ROS 2 topics and services.
     * @param node - pointer to the active RosNode instance.
     */
    void setRosNode(RosNode *node);

public slots:
    /**
     * @brief Updates the UI labels with new odometry and velocity data.
     * @details This slot receives the processed odomState, converts the
     * values (like Yaw to degrees), and updates the text on the labels.
     * @param state - the current state of the robot containing pose and twist.
     */
    void updateOdom(odomState state);

    /**
     * @brief Updates the battery status label.
     * @param voltage - current battery voltage in Volts [V].
     */
    void updateBattery(double voltage);

    /**
     * @brief Updates the camera display view with a new frame.
     * @param img - the incoming camera image to be rendered on the UI.
     */
    void updateCameraImage(const QImage &img);

    /**
     * @brief Refresh all UI text when language changes
     */
    void refreshLanguage();

private:
    QLabel *xLabel;                 /**< Label displaying the X-coordinate [m]. */
    QLabel *yLabel;                 /**< Label displaying the Y-coordinate [m]. */
    QLabel *yawLabel;               /**< Label displaying the orientation (Yaw) [deg]. */

    QLabel *linearVelLabel;         /**< Label displaying forward linear velocity [m/s]. */
    QLabel *angularVelLabel;        /**< Label displaying rotational velocity [deg/s]. */

    QLabel *batteryLabel;           /**< Label displaying the averaged battery voltage. */

    QLabel *cameraLabel;            /**< Placeholder for camera stream visualization. */
    
    QLabel *mainTitleLabel;         /**< Main title label (LEO SYSTEM). */
    
    QLabel *poseTitle;              /**< Title label for pose section. */
    QLabel *velTitle;               /**< Title label for velocity section. */
    
    QLabel *xTitleLabel;            /**< Title label for X axis. */
    QLabel *yTitleLabel;            /**< Title label for Y axis. */
    QLabel *yawTitleLabel;          /**< Title label for Yaw axis. */
    
    QLabel *linTitleLabel;          /**< Title label for Linear velocity. */
    QLabel *angTitleLabel;          /**< Title label for Angular velocity. */

    BatteryWidget *batteryIcon;     /**< Custom widget representing the battery visually. */
    ControlPadWidget *controlPad;   /**< Custom widget for manual robot control. */
    
    RosNode *ros_node_ = nullptr;   /**< Pointer to the underlying ROS 2 node communications manager. */
    
    /**
     * @brief Update displayed text with labels
     */
    void updateLabels();
};

#endif