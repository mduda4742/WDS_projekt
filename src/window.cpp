#include "window.hpp"

Window::Window(QWidget *parent) : QMainWindow(parent) {

    // Set up the main window
    setWindowTitle("Leo Rover Base - Qt GUI");
    resize(800, 600);

    // Create tab widget
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
    auto *title  = new QLabel("<h1>Welcome to the WDS Project Home Page!</h1>", page);
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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map content placeholder on top
    auto *mapPlaceholder = new QLabel("<p>SLAM visualization will go here.</p>", page);
    mapPlaceholder->setStyleSheet("background-color : lightgray; color : white;");
    mapPlaceholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapPlaceholder, 0, 0, 1, 2);
    layout->setRowStretch(0, 3); 

    // Add title on bottom left
    auto *title  = new QLabel("<h3>SLAM Page</h3>", page);
    title -> setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 1, 0);
    layout->setRowStretch(1, 1); 

    // Add gamepad-style control layout on bottom right
    auto *controlsWidget = new QWidget(page);
    auto *controlsLayout = new QGridLayout(controlsWidget);
    controlsLayout->setSpacing(3);
    controlsLayout->setContentsMargins(5, 5, 5, 5);
    
    // Set fixed size for square panel
    controlsWidget->setFixedSize(200, 200);
    controlsWidget->setStyleSheet("border: 1px solid black;");

    // Create all 8 directional buttons
    auto *btnUpLeft = new QPushButton("↖", page);
    auto *btnUp = new QPushButton("↑", page);
    auto *btnUpRight = new QPushButton("↗", page);
    
    auto *btnLeft = new QPushButton("←", page);
    auto *btnCenter = new QPushButton("●", page);
    auto *btnRight = new QPushButton("→", page);
    
    auto *btnDownLeft = new QPushButton("↙", page);
    auto *btnDown = new QPushButton("↓", page);
    auto *btnDownRight = new QPushButton("↘", page);

    // Set button size
    int btnSize = 40;
    btnUpLeft->setFixedSize(btnSize, btnSize);
    btnUp->setFixedSize(btnSize, btnSize);
    btnUpRight->setFixedSize(btnSize, btnSize);
    btnLeft->setFixedSize(btnSize, btnSize);
    btnCenter->setFixedSize(btnSize, btnSize);
    btnRight->setFixedSize(btnSize, btnSize);
    btnDownLeft->setFixedSize(btnSize, btnSize);
    btnDown->setFixedSize(btnSize, btnSize);
    btnDownRight->setFixedSize(btnSize, btnSize);

    // Arrange buttons in 3x3 grid:
    controlsLayout->addWidget(btnUpLeft, 0, 0);
    controlsLayout->addWidget(btnUp, 0, 1);
    controlsLayout->addWidget(btnUpRight, 0, 2);
    
    controlsLayout->addWidget(btnLeft, 1, 0);
    controlsLayout->addWidget(btnCenter, 1, 1);
    controlsLayout->addWidget(btnRight, 1, 2);
    
    controlsLayout->addWidget(btnDownLeft, 2, 0);
    controlsLayout->addWidget(btnDown, 2, 1);
    controlsLayout->addWidget(btnDownRight, 2, 2);

    layout->addWidget(controlsWidget, 1, 1, Qt::AlignRight | Qt::AlignBottom);

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