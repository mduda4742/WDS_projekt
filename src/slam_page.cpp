#include "slam_page.hpp"
#include "ros_node.hpp"
#include "language_manager.hpp"
#include <QPainter>
#include <cmath>
#include <algorithm>

// MapWidget Implementation

/**
 * @brief MapWidget constructor. Initializes the widget with black background for LIDAR visualization.
 * @param parent Parent widget
 */
MapWidget::MapWidget(QWidget *parent) : QWidget(parent), hasData_(false), hasPath_(false), robot_x_(0.0), robot_y_(0.0), robot_theta_(0.0) {
    setStyleSheet("background-color: black;");
    setMinimumSize(400, 400);
}

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
    
    // Draw SLAM path if valid data is available
    if (hasPath_) {
        drawPath(painter);
    }
    
    // Draw LIDAR point cloud if valid data is available
    if (hasData_) {
        drawLaserScan(painter);
    }

    // Draw robot position marker at center (oriented triangle)
    int centerX = width() / 2;
    int centerY = height() / 2;
    painter.save();
    painter.translate(centerX, centerY);
    painter.rotate(-robot_theta_ * 180.0 / M_PI); // Negative for clockwise rotation in Qt
    painter.setPen(QPen(Qt::green, 2));
    painter.setBrush(Qt::green);
    QPointF points[3] = {
        QPointF(15, 0), QPointF(-7, -7), QPointF(-7, 7)
    };
    painter.drawPolygon(points, 3);
    painter.restore();
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
    float maxRange = 10.0f; // Maximum visible range in meters
    float scale = std::min(width(), height()) / (2.0f * maxRange);
    int centerX = width() / 2;
    int centerY = height() / 2;

    QPen linePen(Qt::red, 2);
    QPen pointPen(Qt::cyan, 3);

    QPoint prevPoint(centerX, centerY);
    bool firstPoint = true;
    
    // First pass: draw lines
    painter.setPen(linePen);
    for (size_t i = 0; i < ranges_.size(); ++i) {
        float range = ranges_[i];
        if (!std::isfinite(range) || range <= 0.0f) {
            firstPoint = true;
            continue;
        }
        float angle = angle_min_ + i * angle_increment_ + robot_theta_;
        float x = range * std::cos(angle);
        float y = range * std::sin(angle);
        int screenX = centerX + static_cast<int>(x * scale);
        int screenY = centerY - static_cast<int>(y * scale);
        QPoint currentPoint(screenX, screenY);
        if (!firstPoint) {
            painter.drawLine(prevPoint, currentPoint);
        }
        prevPoint = currentPoint;
        firstPoint = false;
    }
    
    // Second pass: draw points on top
    painter.setPen(pointPen);
    for (size_t i = 0; i < ranges_.size(); ++i) {
        float range = ranges_[i];
        if (!std::isfinite(range) || range <= 0.0f) continue;
        float angle = angle_min_ + i * angle_increment_ + robot_theta_;
        float x = range * std::cos(angle);
        float y = range * std::sin(angle);
        int screenX = centerX + static_cast<int>(x * scale);
        int screenY = centerY - static_cast<int>(y * scale);
        painter.drawPoint(screenX, screenY);
    }
}

/**
 * @brief Draw the SLAM path on the map.
 * 
 * Draws the path as a white/yellow trace showing the robot's traveled route.
 * 
 * @param painter Qt painter object for drawing
 */
void MapWidget::drawPath(QPainter &painter) {
    if (path_x_.empty() || path_y_.empty() || path_x_.size() != path_y_.size()) {
        return;  // No valid path data
    }
    
    painter.setPen(QPen(Qt::yellow, 2));
    float maxRange = 10.0f;  // Same scale as LIDAR visualization
    float scale = std::min(width(), height()) / (2.0f * maxRange);
    int centerX = width() / 2;
    int centerY = height() / 2;

    for (size_t i = 0; i < path_x_.size() - 1; ++i) {
        // Convert world coordinates to screen coordinates relative to robot pose
        int x1 = centerX + static_cast<int>((path_x_[i] - robot_x_) * scale);
        int y1 = centerY - static_cast<int>((path_y_[i] - robot_y_) * scale);
        int x2 = centerX + static_cast<int>((path_x_[i + 1] - robot_x_) * scale);
        int y2 = centerY - static_cast<int>((path_y_[i + 1] - robot_y_) * scale);
        painter.drawLine(x1, y1, x2, y2);
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

/**
 * @brief Store SLAM path data and trigger widget repaint.
 * 
 * @param path_x Vector of X coordinates along the path
 * @param path_y Vector of Y coordinates along the path
 */
void MapWidget::updatePath(const std::vector<double> &path_x, const std::vector<double> &path_y) {
    path_x_ = path_x;
    path_y_ = path_y;
    hasPath_ = true;
    update();  // Queue repaint event
}

void MapWidget::updateRobotPose(double x, double y, double theta) {
    robot_x_ = x;
    robot_y_ = y;
    robot_theta_ = theta;
    update(); // Repaint with new pose
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
    pageTitle = new QLabel("<h3>" + LanguageManager::getInstance().translate("slam_page") + "</h3>", this);
    pageTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(pageTitle, 1, 0);
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

/**
 * @brief Forward SLAM path data to MapWidget for visualization.
 * 
 * @param path_x Vector of X coordinates along the path
 * @param path_y Vector of Y coordinates along the path
 */
void SlamPage::updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y) {
    mapWidget->updatePath(path_x, path_y);
}

void SlamPage::updateRobotPose(double x, double y, double theta) {
    mapWidget->updateRobotPose(x, y, theta);
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

void SlamPage::refreshLanguage() {
    if (pageTitle) {
        pageTitle->setText("<h3>" + LanguageManager::getInstance().translate("slam_page") + "</h3>");
    }
}