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
 * * Displays a grid background, robot center marker, and LIDAR point cloud in real-time.
 * Uses polar-to-Cartesian coordinate conversion for visualization.
 */
class MapWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for MapWidget. Initializes with black background and minimum size.
     * @param[in] parent - parent widget
     */
    explicit MapWidget(QWidget *parent = nullptr);
    virtual ~MapWidget() = default;

protected:
    /**
     * @brief Qt paint event handler. Draws grid, robot marker, and LIDAR data.
     * Called automatically when widget needs repainting.
     * @param[in] event - the paint event
     */
    void paintEvent(QPaintEvent *event) override;

public slots:
    /**
     * @brief Update the widget with new LIDAR scan data and trigger repaint.
     * Stores scan parameters and marks data as available for drawing.
     * @param[in] ranges - vector of distance measurements in meters
     * @param[in] angle_min - minimum scan angle in radians
     * @param[in] angle_max - maximum scan angle in radians
     * @param[in] angle_increment - angular resolution between measurements in radians
     */
    void updateLaserScan(const std::vector<float> &ranges, 
                         float angle_min, float angle_max, float angle_increment);
    
    /**
     * @brief Update the widget with SLAM path data and trigger repaint.
     * Stores the robot's traveled path for visualization.
     * @param[in] path_x - vector of X coordinates along the path
     * @param[in] path_y - vector of Y coordinates along the path
     */
    void updatePath(const std::vector<double> &path_x, const std::vector<double> &path_y);
    
    /**
     * @brief Update the robot's pose on the map.
     * @param[in] x - robot's X coordinate in the map frame
     * @param[in] y - robot's Y coordinate in the map frame
     * @param[in] theta - robot's orientation (yaw) in the map frame
     */
    void updateRobotPose(double x, double y, double theta);

private:
    std::vector<float> ranges_;       ///< LIDAR distance measurements
    float angle_min_;                 ///< Minimum scan angle
    float angle_max_;                 ///< Maximum scan angle
    float angle_increment_;           ///< Angular resolution of scan
    bool hasData_;                    ///< Flag indicating valid data is available
    
    std::vector<double> path_x_;      ///< X coordinates of robot path
    std::vector<double> path_y_;      ///< Y coordinates of robot path
    bool hasPath_;                    ///< Flag indicating valid path data is available
    
    double robot_x_;                  ///< Robot X position in map frame
    double robot_y_;                  ///< Robot Y position in map frame
    double robot_theta_;              ///< Robot orientation (yaw) in map frame
    
    /**
     * @brief Convert polar LIDAR coordinates to Cartesian and draw points.
     * @param[in,out] painter - Qt painter object for drawing. State is modified during drawing operations.
     */
    void drawLaserScan(QPainter &painter);
    
    /**
     * @brief Draw the SLAM path on the map.
     * @param[in,out] painter - Qt painter object for drawing. State is modified during drawing operations.
     */
    void drawPath(QPainter &painter);
};

/**
 * @class SlamPage
 * @brief SLAM (Simultaneous Localization and Mapping) visualization page.
 * * Contains a MapWidget for 2D LIDAR visualization and a 3x3 gamepad control grid
 * for sending movement commands to the robot via cmd_vel topic.
 */
class SlamPage : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructor for SlamPage. Sets up layout with map display and control buttons.
     * @param[in] parent - parent widget
     */
    explicit SlamPage(QWidget *parent = nullptr);
    virtual ~SlamPage() = default;
    
    /**
     * @brief Set the ROS node pointer for publishing velocity commands.
     * Must be called before using movement controls.
     * @param[in] node - pointer to RosNode instance
     */
    void setRosNode(RosNode *node);

public slots:
    /**
     * @brief Receive and display LIDAR scan data on the map widget.
     * Forwards data to MapWidget for visualization.
     * @param[in] ranges - vector of distance measurements
     * @param[in] angle_min - minimum scan angle
     * @param[in] angle_max - maximum scan angle
     * @param[in] angle_increment - angular resolution
     */
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);
    
    /**
     * @brief Receive and display SLAM path data on the map widget.
     * Forwards path data to MapWidget for visualization.
     * @param[in] path_x - vector of X coordinates along the path
     * @param[in] path_y - vector of Y coordinates along the path
     */
    void updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y);
    
    /**
     * @brief Receive and forward robot pose data to the map widget.
     * @param[in] x - robot's X coordinate
     * @param[in] y - robot's Y coordinate
     * @param[in] theta - robot's orientation (yaw)
     */
    void updateRobotPose(double x, double y, double theta);

    /**
     * @brief Refresh all UI text when language changes
     */
    void refreshLanguage();

private:
    MapWidget *mapWidget;            ///< Widget displaying LIDAR data
    RosNode *ros_node_;              ///< ROS node for publishing commands
    ControlPadWidget *controlPad;    ///< Control pad widget for robot movement
};

#endif