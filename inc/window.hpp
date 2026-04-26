#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <vector>
#include "slam_page.hpp"
#include "imu_page.hpp"

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
     * @param parent Parent widget
     */
    Window(QWidget *parent = nullptr);
    /**
     * @brief Destructor for the Window class.
     */
    virtual ~Window() = default;
    
    /**
     * @brief Set the ROS node pointer and forward it to pages for command publishing.
     * Must be called before movement controls will work.
     * @param node Pointer to RosNode instance
     */
    void setRosNode(class RosNode *node);

public slots:
    /**
     * @brief Forward IMU acceleration data to the IMU page for display.
     * @param msg Formatted acceleration information string
     */
    void updateAccelData(const QString &msg);

    /**
     * @brief Forward IMU gyroscope data to the IMU page for display.
     * @param msg Formatted gyroscope information string
     */
    void updateGyroData(const QString &msg);

    /**
     * @brief Forward LIDAR scan data to the SLAM page for visualization.
     * Routes to the map widget for 2D point cloud rendering.
     * @param ranges Vector of distance measurements in meters
     * @param angle_min Minimum scan angle in radians
     * @param angle_max Maximum scan angle in radians
     * @param angle_increment Angular resolution in radians
     */
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);

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
     * @brief Create the Home page with welcome message.
     * @return QWidget pointer to the home page
     */
    QWidget *createHomePage();
};

#endif
