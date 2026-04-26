#include "imu_page.hpp"

/**
 * @brief ImuPage constructor. Creates layout with IMU data display labels.
 * @param parent Parent widget
 */
ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    auto *title = new QLabel("<h1>IMU Page</h1>", this);
    title->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);

    // Add IMU data visualization labels
    accelLabel = new QLabel("Accel: 0.0 : m/s^2", this);
    gyroLabel = new QLabel("Gyro: 0.0 : deg/s", this);
    accelLabel->setAlignment(Qt::AlignCenter);
    gyroLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(accelLabel);
    layout->addWidget(gyroLabel);
}

/**
 * @brief Update acceleration data display with new value.
 * @param msg Formatted acceleration information string
 */
void ImuPage::updateAccelData(const QString &msg) {
    accelLabel->setText("Accel: " + msg);
}

/**
 * @brief Update gyroscope data display with new value.
 * @param msg Formatted gyroscope information string
 */
void ImuPage::updateGyroData(const QString &msg) {
    gyroLabel->setText("Gyro: " + msg);
}
