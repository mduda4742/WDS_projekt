#include "imu_page.hpp"
#include <cmath>

ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *mainHLayout = new QHBoxLayout(this);

    auto *leftLayout = new QVBoxLayout();

    cameraLabel = new QLabel("OCZEKIWANIE NA OBRAZ...", this);
    cameraLabel->setMinimumSize(400, 300);

    cameraLabel->setStyleSheet(
        "background-color: #000000; "
        "color: #555555; "
        "border: 3px solid #333333; "
        "border-radius: 10px; "
        "font-weight: bold;"
        );

    cameraLabel->setAlignment(Qt::AlignCenter);
    cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    leftLayout->addWidget(cameraLabel);
    leftLayout->addStretch();

    mainHLayout->addLayout(leftLayout, 2);

    auto *rightLayout = new QVBoxLayout();

    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel("LEO SYSTEM", this);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #444;");

    batteryLabel = new QLabel("12.4 V", this);
    batteryLabel->setFixedSize(65, 25);
    batteryLabel->setStyleSheet("background: #222; color: #00FF00; border-radius: 3px; font-family: Monospace;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryLabel);
    rightLayout->addLayout(topBar);

    rightLayout->addSpacing(20);

    auto *poseTitle = new QLabel("POSE", this);
    poseTitle->setStyleSheet("color: #2980b9; font-weight: bold;");
    rightLayout->addWidget(poseTitle);

    xLabel   = new QLabel("X:  0.0 m", this);
    yLabel   = new QLabel("Y:  0.0 m", this);
    yawLabel = new QLabel("YAW:  0.0 °", this);

    rightLayout->addWidget(xLabel);
    rightLayout->addWidget(yLabel);
    rightLayout->addWidget(yawLabel);

    rightLayout->addSpacing(20);

    auto *velTitle = new QLabel("VELOCITY", this);
    velTitle->setStyleSheet("color: #c0392b; font-weight: bold;");
    rightLayout->addWidget(velTitle);

    linearVelLabel  = new QLabel("LIN:  0.0 m/s", this);
    angularVelLabel = new QLabel("ANG:  0.0 °/s", this);

    rightLayout->addWidget(linearVelLabel);
    rightLayout->addWidget(angularVelLabel);

    rightLayout->addStretch();

    mainHLayout->addLayout(rightLayout, 1);
}

void ImuPage::updateYaw(double yaw) {
    double yaw_deg = yaw * (180.0 / M_PI);

    yawLabel->setText(QString("YAW:  %1 °").arg(yaw_deg, 0, 'f', 1));
}

void ImuPage::updateVoltage(double voltage) {
    batteryLabel->setText(QString("%1 V").arg(voltage, 0, 'f', 1));
}

/*
void ImuPage::updateAccelData(const QString &msg) {
    accelLabel->setText("Accel: " + msg);
}

void ImuPage::updateGyroData(const QString &msg) {
    gyroLabel->setText("Gyro: " + msg);
}
*/
