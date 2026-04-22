#include "imu_page.hpp"

ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    auto *title = new QLabel("<h1>IMU Page</h1>", this);
    title->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);

    // Add IMU data visualization placeholder
    accelLabel = new QLabel("Accel: 0.0 : m/s^2", this);
    gyroLabel = new QLabel("Gyro: 0.0 : deg/s", this);
    accelLabel->setAlignment(Qt::AlignCenter);
    gyroLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(accelLabel);
    layout->addWidget(gyroLabel);
}

void ImuPage::updateAccelData(const QString &msg) {
    accelLabel->setText("Accel: " + msg);
}

void ImuPage::updateGyroData(const QString &msg) {
    gyroLabel->setText("Gyro: " + msg);
}
