#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QTabWidget>
#include <QPushButton>
#include <vector>
#include "slam_page.hpp"
#include "imu_page.hpp"
#include "odomState.hpp"
#include "language_manager.hpp"

/**
 * @class Window
 * @brief Main application window and central signal routing hub.
 * 
 * This is the primary GUI container with a tab-based interface for different
 * functionalities: Home (welcome), SLAM (LIDAR visualization), and IMU (sensor data).
 * Routes data signals from ROS node callbacks to appropriate page display handlers.
 */
class Window : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the Window class. Initializes the main window with title,
     * size, and creates three tabs: Home page, SLAM page, and IMU page.
     * @param parent - pointer to the parent widget
     */
    Window(QWidget *parent = nullptr);
    /**
     * @brief Destructor for the Window class.
     */
    virtual ~Window() = default;
    
    /**
     * @brief Set the ROS node pointer and forward it to pages for command publishing.
     * Must be called before movement controls will work.
     * @param node - pointer to RosNode instance
     */
    void setRosNode(class RosNode *node);

public slots:
    /**
     * @brief Slot to update the Odometry and Robot State data.
     * @param state - the current pose and velocity state of the robot.
     */
    void updateOdomData(odomState state);

    /**
     * @brief Slot to update the battery voltage display.
     * @param voltage - current battery voltage in Volts [V].
     */
    void updateBatteryData(double voltage);

    /**
     * @brief Slot to receive the latest camera image frame and forward it to the IMU page display.
     * @param img - the incoming camera image frame.
     */
    void updateCameraImage(const QImage &img);

    /**
     * @brief Forward LIDAR scan data to the SLAM page for visualization.
     * Routes to the map widget for 2D point cloud rendering.
     * @param ranges - vector of distance measurements in meters
     * @param angle_min - minimum scan angle in radians
     * @param angle_max - maximum scan angle in radians
     * @param angle_increment - angular resolution in radians
     */
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);
    
    /**
     * @brief Forward SLAM path data to the SLAM page for visualization.
     * Routes to the map widget for path rendering.
     * @param path_x - vector of X coordinates along the path
     * @param path_y - vector of Y coordinates along the path
     */
    void updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y);

    /**
     * @brief Refresh all UI text when language changes
     */
    void refreshLanguage();

private:
    /**
     * @brief Pointer to the QTabWidget that manages Home, SLAM, and IMU pages.
     */
    QTabWidget *tabWidget;

    /**
     * @brief Pointer to the SLAM visualization page.
     */
    SlamPage *slamPage;

    /**
     * @brief Pointer to the IMU sensor data display page.
     */
    ImuPage *imuPage;

    /**
     * @brief Label for home page title
     */
    QLabel *homeTitle;

    /**
     * @brief Label for home page subtitle
     */
    QLabel *homeSubtitle;

    /**
     * @brief Create the Home page with welcome message and language selector.
     * @return QWidget pointer to the home page
     */
    QWidget *createHomePage();
};

#endif
