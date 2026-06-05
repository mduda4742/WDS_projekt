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
SlamPage::SlamPage(QWidget *parent) : QWidget(parent), ros_node_(nullptr) {
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map widget on top (takes most of the screen for LIDAR visualization)
    mapWidget = new MapWidget(this);
    layout->addWidget(mapWidget, 0, 0, 1, 2);
    layout->setRowStretch(0, 3);

    // Add title on bottom left
    pageTitle = new QLabel("<h3>" + LanguageManager::getInstance().translate("slam_page_title_en") + "</h3>", this);
    pageTitle->setAlignment(Qt::AlignCenter);
    layout->addWidget(pageTitle, 1, 0);
    layout->setRowStretch(1, 1);

    // Add control pad on bottom right
    controlPad = new ControlPadWidget(this);
    layout->addWidget(controlPad, 1, 1, Qt::AlignRight | Qt::AlignBottom);
}

/**
 * @brief Create and configure 3x3 gamepad-style button grid for directional movement.
 * 
 * Buttons change from green to orange when pressed for visual feedback.
 * Each button is connected to its corresponding movement handler slot.
 * 
 * @param layout Grid layout to add buttons to
 */
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

void SlamPage::setRosNode(RosNode *node) {
    ros_node_ = node;
    
    if (ros_node_ && controlPad) {
        connect(controlPad, &ControlPadWidget::velocityRequested,
                ros_node_, &RosNode::publishVelocity);
    }
}

void SlamPage::refreshLanguage() {
    if (pageTitle) {
        pageTitle->setText("<h3>" + LanguageManager::getInstance().translate("slam_page") + "</h3>");
    }
}