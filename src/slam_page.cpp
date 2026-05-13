#include "slam_page.hpp"
#include "ros_node.hpp"
#include <QPainter>
#include <cmath>
#include <algorithm>

// MapWidget Implementation

/**
 * @brief MapWidget constructor. Initializes the widget with black background for LIDAR visualization.
 * @param parent Parent widget
 */
MapWidget::MapWidget(QWidget *parent) : QWidget(parent), hasData_(false) {
    setStyleSheet("background-color: black;");
    setMinimumSize(400, 400);
}

/**
 * @brief Paint event handler. Renders the 2D LIDAR map with grid, robot position, and point cloud.
 * @param event The paint event (unused but required by Qt API)
 */
void MapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw black background
    painter.fillRect(rect(), Qt::black);
    
    // Draw reference grid (50 pixel spacing = ~1 meter at default scale)
    painter.setPen(QPen(Qt::darkGray, 1));
    int gridSpacing = 50;
    for (int x = 0; x < width(); x += gridSpacing) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSpacing) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw robot position marker at center (green circle)
    painter.setPen(QPen(Qt::green, 4));
    int centerX = width() / 2;
    int centerY = height() / 2;
    painter.drawPoint(centerX, centerY);
    painter.drawEllipse(centerX - 10, centerY - 10, 20, 20);
    
    // Draw LIDAR point cloud if valid data is available
    if (hasData_) {
        drawLaserScan(painter);
    }
}

/**
 * @brief Convert polar LIDAR coordinates to Cartesian and draw points with connecting lines.
 * 
 * Red lines connect consecutive measurements, cyan dots mark individual points.
 * Invalid measurements (NaN, infinity, or zero) are skipped.
 * 
 * @param painter Qt painter object for drawing
 */
void MapWidget::drawLaserScan(QPainter &painter) {
    // Draw connecting lines between consecutive LIDAR measurements
    painter.setPen(QPen(Qt::red, 2));
    
    float maxRange = 10.0f; // Maximum visible range in meters
    float scale = std::min(width(), height()) / (2.0f * maxRange);
    
    int centerX = width() / 2;
    int centerY = height() / 2;
    
    QPoint prevPoint(centerX, centerY);
    bool firstPoint = true;
    
    for (size_t i = 0; i < ranges_.size(); ++i) {
        float range = ranges_[i];
        
        // Skip invalid readings (infinity or NaN)
        if (!std::isfinite(range) || range <= 0.0f) {
            firstPoint = true;
            continue;
        }
        
        // Calculate angle for this reading
        float angle = angle_min_ + i * angle_increment_;
        
        // Convert to Cartesian coordinates (with y-axis pointing up in map)
        float x = range * std::cos(angle);
        float y = range * std::sin(angle);
        
        // Scale and convert to screen coordinates
        int screenX = centerX + static_cast<int>(x * scale);
        int screenY = centerY - static_cast<int>(y * scale);  // Invert Y for screen coords
        
        QPoint currentPoint(screenX, screenY);
        
        if (!firstPoint) {
            painter.drawLine(prevPoint, currentPoint);
        }
        
        prevPoint = currentPoint;
        firstPoint = false;
    }
    
    // Draw cyan points for each individual LIDAR measurement
    painter.setPen(QPen(Qt::cyan, 3));
    for (size_t i = 0; i < ranges_.size(); ++i) {
        float range = ranges_[i];
        
        // Skip invalid measurements (infinity or NaN or zero)
        if (!std::isfinite(range) || range <= 0.0f) {
            continue;
        }
        
        // Calculate angle for this measurement
        float angle = angle_min_ + i * angle_increment_;
        
        // Convert polar (angle, range) to Cartesian (x, y)
        float x = range * std::cos(angle);
        float y = range * std::sin(angle);
        
        // Scale and translate to screen coordinates
        int screenX = centerX + static_cast<int>(x * scale);
        int screenY = centerY - static_cast<int>(y * scale);
        
        painter.drawPoint(screenX, screenY);
    }
}

/**
 * @brief Store LIDAR scan data and trigger widget repaint.
 * 
 * @param ranges Vector of distance measurements in meters
 * @param angle_min Minimum scan angle in radians
 * @param angle_max Maximum scan angle in radians
 * @param angle_increment Angular resolution between measurements in radians
 */
void MapWidget::updateLaserScan(const std::vector<float> &ranges, 
                                 float angle_min, float angle_max, float angle_increment) {
    ranges_ = ranges;
    angle_min_ = angle_min;
    angle_max_ = angle_max;
    angle_increment_ = angle_increment;
    hasData_ = true;
    update();  // Queue repaint event
}

// SlamPage Implementation

/**
 * @brief SlamPage constructor. Sets up layout with MapWidget for visualization and 
 * 3x3 gamepad grid for movement controls.
 * @param parent Parent widget
 */
SlamPage::SlamPage(QWidget *parent) : QWidget(parent), ros_node_(nullptr), 
                                       linear_speed_(0.5), angular_speed_(0.5) {
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map widget on top (takes 3/4 of the screen for LIDAR visualization)
    mapWidget = new MapWidget(this);
    layout->addWidget(mapWidget, 0, 0, 1, 2);
    layout->setRowStretch(0, 3);

    // Add title on bottom left
    auto *title = new QLabel("<h3>SLAM Page</h3>", this);
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 1, 0);
    layout->setRowStretch(1, 1);

    // Add gamepad-style control layout on bottom right
    auto *controlsWidget = new QWidget(this);
    auto *controlsLayout = new QGridLayout(controlsWidget);
    controlsLayout->setSpacing(3);
    controlsLayout->setContentsMargins(5, 5, 5, 5);

    // Set fixed size for square panel
    controlsWidget->setFixedSize(200, 200);
    controlsWidget->setStyleSheet("border: 1px solid black;");

    setupControls(controlsLayout);

    layout->addWidget(controlsWidget, 1, 1, Qt::AlignRight | Qt::AlignBottom);
}

/**
 * @brief Create and configure 3x3 gamepad-style button grid for directional movement.
 * 
 * Buttons change from green to orange when pressed for visual feedback.
 * Each button is connected to its corresponding movement handler slot.
 * 
 * @param layout Grid layout to add buttons to
 */
void SlamPage::setupControls(QGridLayout *layout) {
    // Create all 9 directional buttons with arrow symbols
    auto *btnUpLeft = new QPushButton("↖", this);
    auto *btnUp = new QPushButton("↑", this);
    auto *btnUpRight = new QPushButton("↗", this);

    auto *btnLeft = new QPushButton("←", this);
    auto *btnCenter = new QPushButton("●", this);
    auto *btnRight = new QPushButton("→", this);

    auto *btnDownLeft = new QPushButton("↙", this);
    auto *btnDown = new QPushButton("↓", this);
    auto *btnDownRight = new QPushButton("↘", this);

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

    // Set button stylesheets for visual feedback
    const QString buttonStyle = R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: 2px solid #2E7D32;
            border-radius: 5px;
            font-weight: bold;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #ff9800;
            border: 2px solid #f57c00;
        }
    )";
    
    btnUpLeft->setStyleSheet(buttonStyle);
    btnUp->setStyleSheet(buttonStyle);
    btnUpRight->setStyleSheet(buttonStyle);
    btnLeft->setStyleSheet(buttonStyle);
    btnCenter->setStyleSheet(buttonStyle);
    btnRight->setStyleSheet(buttonStyle);
    btnDownLeft->setStyleSheet(buttonStyle);
    btnDown->setStyleSheet(buttonStyle);
    btnDownRight->setStyleSheet(buttonStyle);

    // Connect button signals to movement handlers
    connect(btnUp, &QPushButton::pressed, this, &SlamPage::handleMoveForward);
    connect(btnUp, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnDown, &QPushButton::pressed, this, &SlamPage::handleMoveBackward);
    connect(btnDown, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnLeft, &QPushButton::pressed, this, &SlamPage::handleMoveLeft);
    connect(btnLeft, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnRight, &QPushButton::pressed, this, &SlamPage::handleMoveRight);
    connect(btnRight, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnUpLeft, &QPushButton::pressed, this, &SlamPage::handleMoveForwardLeft);
    connect(btnUpLeft, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnUpRight, &QPushButton::pressed, this, &SlamPage::handleMoveForwardRight);
    connect(btnUpRight, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnDownLeft, &QPushButton::pressed, this, &SlamPage::handleMoveBackwardLeft);
    connect(btnDownLeft, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnDownRight, &QPushButton::pressed, this, &SlamPage::handleMoveBackwardRight);
    connect(btnDownRight, &QPushButton::released, this, &SlamPage::handleStop);
    
    connect(btnCenter, &QPushButton::pressed, this, &SlamPage::handleStop);

    // Arrange buttons in 3x3 grid:
    // ↖ ↑ ↗
    // ← ● →
    // ↙ ↓ ↘
    layout->addWidget(btnUpLeft, 0, 0);
    layout->addWidget(btnUp, 0, 1);
    layout->addWidget(btnUpRight, 0, 2);

    layout->addWidget(btnLeft, 1, 0);
    layout->addWidget(btnCenter, 1, 1);
    layout->addWidget(btnRight, 1, 2);

    layout->addWidget(btnDownLeft, 2, 0);
    layout->addWidget(btnDown, 2, 1);
    layout->addWidget(btnDownRight, 2, 2);
}

/**
 * @brief Forward LIDAR scan data to MapWidget for visualization.
 * 
 * @param ranges Vector of distance measurements in meters
 * @param angle_min Minimum scan angle in radians
 * @param angle_max Maximum scan angle in radians
 * @param angle_increment Angular resolution in radians
 */
void SlamPage::updateLaserData(const std::vector<float> &ranges, 
                               float angle_min, float angle_max, float angle_increment) {
    mapWidget->updateLaserScan(ranges, angle_min, angle_max, angle_increment);
}

/// @brief Send forward velocity command to robot
void SlamPage::handleMoveForward() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send backward velocity command to robot
void SlamPage::handleMoveBackward() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send left strafe velocity command to robot
void SlamPage::handleMoveLeft() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send right strafe velocity command to robot
void SlamPage::handleMoveRight() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send counter-clockwise rotation command to robot
void SlamPage::handleTurnLeft() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send clockwise rotation command to robot
void SlamPage::handleTurnRight() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send forward-left diagonal movement command
void SlamPage::handleMoveForwardLeft() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send forward-right diagonal movement command
void SlamPage::handleMoveForwardRight() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send backward-left diagonal movement command
void SlamPage::handleMoveBackwardLeft() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send backward-right diagonal movement command
void SlamPage::handleMoveBackwardRight() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}

/// @brief Send zero velocity command to stop the robot
void SlamPage::handleStop() {
    if (ros_node_) {
        ros_node_->publishVelocity(0.0, 0.0);
    }
}