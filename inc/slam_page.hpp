#ifndef SLAM_PAGE_HPP
#define SLAM_PAGE_HPP

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QPainter>
#include <vector>

#include "control_pad_widget.hpp"

class RosNode;  // Forward declaration

/**
 * @class MapWidget
 * @brief Custom QWidget that renders 2D LIDAR scan data with robot position visualization.
 * 
 * Displays a grid background, robot center marker, and LIDAR point cloud in real-time.
 * Uses polar-to-Cartesian coordinate conversion for visualization.
 */
class MapWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for MapWidget. Initializes with black background and minimum size.
     * @param parent Parent widget
     */
    explicit MapWidget(QWidget *parent = nullptr);
    virtual ~MapWidget() = default;

protected:
    /**
     * @brief Qt paint event handler. Draws grid, robot marker, and LIDAR data.
     * Called automatically when widget needs repainting.
     * @param event The paint event
     */
    void paintEvent(QPaintEvent *event) override;

public slots:
    /**
     * @brief Update the widget with new LIDAR scan data and trigger repaint.
     * Stores scan parameters and marks data as available for drawing.
     * @param ranges Vector of distance measurements in meters
     * @param angle_min Minimum scan angle in radians
     * @param angle_max Maximum scan angle in radians
     * @param angle_increment Angular resolution between measurements in radians
     */
    void updateLaserScan(const std::vector<float> &ranges, 
                         float angle_min, float angle_max, float angle_increment);

private:
    std::vector<float> ranges_;      ///< LIDAR distance measurements
    float angle_min_;                 ///< Minimum scan angle
    float angle_max_;                 ///< Maximum scan angle
    float angle_increment_;           ///< Angular resolution of scan
    bool hasData_;                    ///< Flag indicating valid data is available
    
    /**
     * @brief Convert polar LIDAR coordinates to Cartesian and draw points and connections.
     * @param painter Qt painter object for drawing
     */
    void drawLaserScan(QPainter &painter);
};

/**
 * @class SlamPage
 * @brief SLAM (Simultaneous Localization and Mapping) visualization page.
 * 
 * Contains a MapWidget for 2D LIDAR visualization and a 3x3 gamepad control grid
 * for sending movement commands to the robot via cmd_vel topic.
 */
class SlamPage : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for SlamPage. Sets up layout with map display and control buttons.
     * @param parent Parent widget
     */
    explicit SlamPage(QWidget *parent = nullptr);
    virtual ~SlamPage() = default;
    
    /**
     * @brief Set the ROS node pointer for publishing velocity commands.
     * Must be called before using movement controls.
     * @param node Pointer to RosNode instance
     */
    void setRosNode(RosNode *node) { ros_node_ = node; }

public slots:
    /**
     * @brief Receive and display LIDAR scan data on the map widget.
     * Forwards data to MapWidget for visualization.
     * @param ranges Vector of distance measurements
     * @param angle_min Minimum scan angle
     * @param angle_max Maximum scan angle
     * @param angle_increment Angular resolution
     */
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);

private slots:
    void handleMoveForward();        ///< Send forward velocity command
    void handleMoveBackward();       ///< Send backward velocity command
    void handleMoveLeft();           ///< Send left strafe velocity command
    void handleMoveRight();          ///< Send right strafe velocity command
    void handleTurnLeft();           ///< Send counter-clockwise rotation command
    void handleTurnRight();          ///< Send clockwise rotation command
    void handleMoveForwardLeft();    ///< Send forward-left diagonal command
    void handleMoveForwardRight();   ///< Send forward-right diagonal command
    void handleMoveBackwardLeft();   ///< Send backward-left diagonal command
    void handleMoveBackwardRight();  ///< Send backward-right diagonal command
    void handleStop();               ///< Send zero velocity command (stop robot)

private:
    MapWidget *mapWidget;            ///< Widget displaying LIDAR data
    RosNode *ros_node_;              ///< ROS node for publishing commands
    double linear_speed_;            ///< Linear velocity magnitude (m/s)
    double angular_speed_;           ///< Angular velocity magnitude (rad/s)

    /**
     * @brief Create and configure 3x3 gamepad control button grid.
     * Connects button signals to movement handler slots.
     * @param layout Grid layout to add buttons to
     */
    void setupControls(QGridLayout *layout);
};

#endif
