#include "window.hpp"

Window::Window(QWidget *parent) : QMainWindow(parent) {

    // Set up the main window
    setWindowTitle("Leo Rover Base - Qt GUI");
    resize(800, 600);

    // Create tab widget (handles both tabs and pages automatically)
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createHomePage(), "Home");
    tabWidget->addTab(createSlamPage(), "SLAM");
    tabWidget->addTab(createImuPage(), "IMU");
    
    setCentralWidget(tabWidget);
}

QWidget* Window::createHomePage() {
    // Create a simple home page with a welcome message
    auto *page   = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    auto *title  = new QLabel("<h1>Welcome to the Home Page!</h1>", page);
    auto *subtitle = new QLabel("<p>Authors: Radosław Mijał & Mateusz Duda</p>", page);
    title -> setAlignment(Qt::AlignCenter);
    subtitle -> setAlignment(Qt::AlignCenter);

    layout-> addWidget(title);
    layout-> addWidget(subtitle);

    return page;
}

QWidget* Window::createSlamPage() {
    // Create a simple SLAM page
    auto *page   = new QWidget(this);
    auto *layout = new QGridLayout(page);
    auto *title  = new QLabel("<h1>SLAM Page</h1>", page);
    title -> setAlignment(Qt::AlignCenter);

    layout->addWidget(title, 0, 0, 1, 2);

    // Add map content placeholder
    auto *mapPlaceholder = new QLabel("<p>SLAM visualization will go here.</p>", page);
    mapPlaceholder->setStyleSheet("background-color : lightgray; color : white;");
    mapPlaceholder->setAlignment(Qt::AlignCenter);

    layout->addWidget(mapPlaceholder, 1, 0, 1, 2);

    // Add controls buttons
    auto *btnForward = new QPushButton("Forward", page);
    auto *btnBackward = new QPushButton("Backward", page);
    auto *btnLeft = new QPushButton("Left", page);
    auto *btnRight = new QPushButton("Right", page);
    auto *controlsLayout = new QHBoxLayout();

    controlsLayout->addWidget(btnForward);
    controlsLayout->addWidget(btnBackward);
    controlsLayout->addWidget(btnLeft);
    controlsLayout->addWidget(btnRight);

    layout->addLayout(controlsLayout, 2, 0, 1, 2); 

    return page;
}

QWidget* Window::createImuPage() {
    // Create a simple IMU page
    auto *page   = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    auto *title  = new QLabel("<h1>IMU Page</h1>", page);
    title -> setAlignment(Qt::AlignCenter);

    layout-> addWidget(title);

    // Add IMU data visualization placeholder
    accelLabel = new QLabel("Accel: 0.0 : m/s^2", page);
    gyroLabel  = new QLabel("Gyro: 0.0 : deg/s", page);
    accelLabel->setAlignment(Qt::AlignCenter);
    gyroLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(accelLabel);
    layout->addWidget(gyroLabel);

    return page;
}