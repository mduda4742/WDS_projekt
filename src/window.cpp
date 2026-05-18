#include "window.hpp"
#include "ros_node.hpp"
#include <QVBoxLayout>
#include <QLabel>


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

void Window::updateOdomData(odomState state) {
    imuPage->updateOdom(state);
}

void Window::updateBatteryData(double voltage) {
    imuPage->updateBattery(voltage);
}

void Window::updateCameraImage(const QImage &image) {
    if (imuPage) {
        imuPage->updateCameraImage(image);
    }
}

void Window::updateLaserData(const std::vector<float> &ranges, 
                            float angle_min, float angle_max, float angle_increment) {
    slamPage->updateLaserData(ranges, angle_min, angle_max, angle_increment);
}

void Window::updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y) {
    slamPage->updatePathData(path_x, path_y);
}

void Window::setRosNode(RosNode *node) {
    slamPage->setRosNode(node);
    imuPage->setRosNode(node);
}
