#include "window.hpp"
#include "ros_node.hpp"
#include <QVBoxLayout>
#include <QLabel>

/**
 * @brief Window constructor. Initializes main window with title, size, and three tabs:
 * Home (welcome page), SLAM (LIDAR visualization), and IMU (sensor data display).
 * @param parent Parent widget
 */
Window::Window(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Leo Rover Base - Qt GUI");
    resize(800, 600);

    // Create tab widget to hold all pages
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createHomePage(), "Home");
    
    // Create SLAM page for LIDAR visualization and movement controls
    slamPage = new SlamPage(this);
    tabWidget->addTab(slamPage, "SLAM");
    
    // Create IMU page for sensor data display
    imuPage = new ImuPage(this);
    tabWidget->addTab(imuPage, "IMU");
    
    setCentralWidget(tabWidget);
}

/**
 * @brief Create home page with welcome message.
 * @return QWidget pointer to the home page
 */
QWidget* Window::createHomePage() {
    auto *page = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    auto *title = new QLabel("<h1>Welcome to the WDS Project Home Page!</h1>", page);
    auto *subtitle = new QLabel("<p>Authors: Radosław Mijał & Mateusz Duda</p>", page);
    title->setAlignment(Qt::AlignCenter);
    subtitle->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);
    layout->addWidget(subtitle);

    return page;
}

void Window::updateBatteryData(double voltage) {
    imuPage->updateVoltage(voltage);
}

void Window::updateYawData(double yaw) {
    imuPage->updateYaw(yaw);
}

void Window::updateRobotPose(double x, double y, double theta) {
    (void)theta; // theta is unused, yaw is updated from a separate signal
    imuPage->updatePosition(x, y);
}

/**
 * @brief Forward LIDAR scan data to SLAM page for visualization.
 * 
 * @param ranges Vector of distance measurements in meters
 * @param angle_min Minimum scan angle in radians
 * @param angle_max Maximum scan angle in radians
 * @param angle_increment Angular resolution in radians
 */
void Window::updateLaserData(const std::vector<float> &ranges, 
                            float angle_min, float angle_max, float angle_increment) {
    slamPage->updateLaserData(ranges, angle_min, angle_max, angle_increment);
}

/**
 * @brief Forward SLAM path data to SLAM page for visualization.
 * 
 * @param path_x Vector of X coordinates along the path
 * @param path_y Vector of Y coordinates along the path
 */
void Window::updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y) {
    slamPage->updatePathData(path_x, path_y);
}

/// @brief Set ROS node pointer so pages can publish movement commands
void Window::setRosNode(RosNode *node) {
    slamPage->setRosNode(node);
}