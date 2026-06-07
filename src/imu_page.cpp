#include "imu_page.hpp"
#include "battery_widget.hpp"
#include "control_pad_widget.hpp"
#include "language_manager.hpp"
#include <cmath>
#include <QVBoxLayout>
#include "ros_node.hpp"

ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *mainHLayout = new QHBoxLayout(this);

    auto *leftLayout = new QVBoxLayout();

    view25d = new View25DWidget(this);
    view25d->setMinimumSize(400, 300);
    view25d->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    leftLayout->addWidget(view25d);
    leftLayout->addStretch();

    mainHLayout->addLayout(leftLayout, 2);

    auto *rightLayout = new QVBoxLayout();

    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel(LanguageManager::getInstance().translate("imu_title"), this);
    title->setStyleSheet("font-weight: bold;");

    batteryIcon = new BatteryWidget(this);

    batteryLabel = new QLabel("12.4 V", this);
    batteryLabel->setFixedSize(120, 45);
    batteryLabel->setStyleSheet("background: #222; color: white; border-radius: 3px; font-family: Monospace;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryIcon);
    topBar->addWidget(batteryLabel);

    rightLayout->addLayout(topBar);

    rightLayout->addSpacing(20);

    poseTitle = new QLabel(LanguageManager::getInstance().translate("imu_pose_title"), this);
    poseTitle->setStyleSheet("font-weight: bold;");
    rightLayout->addWidget(poseTitle);

    xLabel   = new QLabel("X:  0.0 m", this);
    yLabel   = new QLabel("Y:  0.0 m", this);
    yawLabel = new QLabel("YAW:  0.0 °", this);

    rightLayout->addWidget(xLabel);
    rightLayout->addWidget(yLabel);
    rightLayout->addWidget(yawLabel);

    rightLayout->addSpacing(20);

    velTitle = new QLabel(LanguageManager::getInstance().translate("imu_velocity_title"), this);
    velTitle->setStyleSheet("font-weight: bold;");
    rightLayout->addWidget(velTitle);

    linearVelLabel  = new QLabel("LIN:  0.0 m/s", this);
    angularVelLabel = new QLabel("ANG:  0.0 °/s", this);

    rightLayout->addWidget(linearVelLabel);
    rightLayout->addWidget(angularVelLabel);

    rightLayout->addStretch();

    controlPad = new ControlPadWidget(this);
    rightLayout->addWidget(controlPad, 0, Qt::AlignRight | Qt::AlignBottom);

    mainHLayout->addLayout(rightLayout, 1);
}

void ImuPage::updateOdom(odomState state) {
    double yaw_deg = state.yaw * (180.0 / M_PI);
    double angular_vel_deg = state.angular_vel * (180.0 / M_PI);

    xLabel->setText(QString("%1:  %2 %3").arg(LanguageManager::getInstance().translate("imu_x_label"), 
                                             QString::number(state.x, 'f', 1),
                                             LanguageManager::getInstance().translate("imu_unit_meters")));
    yLabel->setText(QString("%1:  %2 %3").arg(LanguageManager::getInstance().translate("imu_y_label"), 
                                             QString::number(state.y, 'f', 1),
                                             LanguageManager::getInstance().translate("imu_unit_meters")));
    yawLabel->setText(QString("%1:  %2 %3").arg(LanguageManager::getInstance().translate("imu_yaw_label"), 
                                               QString::number(yaw_deg, 'f', 1),
                                               LanguageManager::getInstance().translate("imu_unit_degrees")));

    linearVelLabel->setText(QString("%1:  %2 %3").arg(LanguageManager::getInstance().translate("imu_linear_vel"), 
                                                     QString::number(state.linear_vel, 'f', 1),
                                                     LanguageManager::getInstance().translate("imu_unit_ms")));
    angularVelLabel->setText(QString("%1:  %2 %3").arg(LanguageManager::getInstance().translate("imu_angular_vel"), 
                                                      QString::number(angular_vel_deg, 'f', 1),
                                                      LanguageManager::getInstance().translate("imu_unit_degps")));
}

void ImuPage::updateBattery(double voltage) {
    batteryLabel->setText(QString("%1 V").arg(voltage, 0, 'f', 1));

    float minVolt = 9.0;
    float maxVolt = 12.6;

    double level = (voltage - minVolt) / (maxVolt - minVolt);

    level = qBound(0.0, level, 1.0);

    batteryIcon->setLevel(level);

    if (level <= 0.2) {
        batteryLabel->setStyleSheet("color: white; background: #AA0000; font-family: Monospace; font-weight: bold;");
    } else {
        batteryLabel->setStyleSheet("color: white; background: #222; font-family: Monospace; font-weight: bold;");
    }
}

void ImuPage::updateCameraImage(const QImage &image) {
    if (!image.isNull() && view25d) {
        view25d->updateImage(image);
    }
}

void ImuPage::updateLaserScan(const std::vector<float> &ranges, float angle_min, float angle_max, float angle_increment) {
    if (view25d) {
        view25d->updateLaserScan(ranges, angle_min, angle_max, angle_increment);
    }
}

void ImuPage::setRosNode(RosNode *node) {
    ros_node_ = node;

    if (ros_node_ && controlPad) {
        connect(controlPad, &ControlPadWidget::velocityRequested, 
                ros_node_, &RosNode::publishVelocity);
    }
}

void ImuPage::refreshLanguage() {
    // Update title labels
    poseTitle->setText(LanguageManager::getInstance().translate("imu_pose_title"));
    velTitle->setText(LanguageManager::getInstance().translate("imu_velocity_title"));
    
    // Update odom labels with fresh translations
    // TODO: Store last odom state to refresh display with new language
}

void ImuPage::updateLabels() {
    // TODO: Implement label updates for dynamic translation
}
