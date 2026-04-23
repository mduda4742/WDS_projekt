#include "slam_page.hpp"
#include <QPainter>
#include <cmath>
#include <algorithm>

// MapWidget Implementation
MapWidget::MapWidget(QWidget *parent) : QWidget(parent), hasData_(false) {
    setStyleSheet("background-color: black;");
    setMinimumSize(400, 400);
}

void MapWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Draw background
    painter.fillRect(rect(), Qt::black);
    
    // Draw grid
    painter.setPen(QPen(Qt::darkGray, 1));
    int gridSpacing = 50;
    for (int x = 0; x < width(); x += gridSpacing) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y < height(); y += gridSpacing) {
        painter.drawLine(0, y, width(), y);
    }
    
    // Draw center point (robot position)
    painter.setPen(QPen(Qt::green, 4));
    int centerX = width() / 2;
    int centerY = height() / 2;
    painter.drawPoint(centerX, centerY);
    painter.drawEllipse(centerX - 10, centerY - 10, 20, 20);
    
    // Draw LIDAR scan if data is available
    if (hasData_) {
        drawLaserScan(painter);
    }
}

void MapWidget::drawLaserScan(QPainter &painter) {
    painter.setPen(QPen(Qt::red, 2));
    
    float maxRange = 10.0f; // meters
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
    
    // Draw points
    painter.setPen(QPen(Qt::cyan, 3));
    for (size_t i = 0; i < ranges_.size(); ++i) {
        float range = ranges_[i];
        
        if (!std::isfinite(range) || range <= 0.0f) {
            continue;
        }
        
        float angle = angle_min_ + i * angle_increment_;
        float x = range * std::cos(angle);
        float y = range * std::sin(angle);
        
        int screenX = centerX + static_cast<int>(x * scale);
        int screenY = centerY - static_cast<int>(y * scale);
        
        painter.drawPoint(screenX, screenY);
    }
}

void MapWidget::updateLaserScan(const std::vector<float> &ranges, 
                                 float angle_min, float angle_max, float angle_increment) {
    ranges_ = ranges;
    angle_min_ = angle_min;
    angle_max_ = angle_max;
    angle_increment_ = angle_increment;
    hasData_ = true;
    update();  // Trigger repaint
}

// SlamPage Implementation
SlamPage::SlamPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map widget on top (takes 3/4 of the screen)
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

void SlamPage::setupControls(QGridLayout *layout) {
    // Create all 8 directional buttons
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

void SlamPage::updateLaserData(const std::vector<float> &ranges, 
                               float angle_min, float angle_max, float angle_increment) {
    mapWidget->updateLaserScan(ranges, angle_min, angle_max, angle_increment);
}