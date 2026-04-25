#include "window.hpp"
#include <QVBoxLayout>
#include <QLabel>

Window::Window(QWidget *parent) : QMainWindow(parent) {
    // Set up the main window
    setWindowTitle("Leo Rover Base - Qt GUI");
    resize(800, 600);

    // Create tab widget
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createHomePage(), "Home");
    
    // Create SLAM page and add to tab
    slamPage = new SlamPage(this);
    tabWidget->addTab(slamPage, "SLAM");
    
    // Create IMU page and add to tab
    imuPage = new ImuPage(this);
    tabWidget->addTab(imuPage, "IMU");
    
    setCentralWidget(tabWidget);
}

QWidget* Window::createHomePage() {
    // Create a simple home page with a welcome message
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

void Window::updateAccelData(const QString &msg) {
    imuPage->updateAccelData(msg);
}

void Window::updateGyroData(const QString &msg) {
    imuPage->updateGyroData(msg);
}

void Window::updateLaserData(const std::vector<float> &ranges, 
                            float angle_min, float angle_max, float angle_increment) {
    slamPage->updateLaserData(ranges, angle_min, angle_max, angle_increment);
}
