#include "imu_page.hpp"
#include "battery_widget.hpp"
#include "control_pad_widget.hpp"
#include "language_manager.hpp"
#include "gl_rover_widget.hpp"
#include <cmath>
#include <QVBoxLayout>
#include "ros_node.hpp"


ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *mainHLayout = new QHBoxLayout(this);

    auto *leftLayout = new QVBoxLayout();

    // cameraWidget (z pełnym tłumaczeniem i jednym, poprawnym wywołaniem)
    cameraLabel = new QLabel(LanguageManager::getInstance().translate("imu_camera_waiting"), this);
    cameraLabel->setMinimumSize(400, 300);
    cameraLabel->setAlignment(Qt::AlignCenter);
    cameraLabel->setStyleSheet(
        "background-color: #000000; "
        "color: white; "
        "border: 3px solid #333333; "
        "border-radius: 10px; "
        "font-weight: bold;"
    );
    cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftLayout->addWidget(cameraLabel);

    leftLayout->addStretch();
    mainHLayout->addLayout(leftLayout, 60);

    auto *rightLayout = new QVBoxLayout();
    auto *topBar = new QHBoxLayout();

    auto *title = new QLabel(LanguageManager::getInstance().translate("imu_title"), this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #444;");

    // battery
    batteryIcon = new BatteryWidget(this);

    batteryLabel = new QLabel("12.4", this);
    batteryLabel->setFixedSize(50, 25); 
    batteryLabel->setStyleSheet("background: #222; color: #00FF00; border-radius: 3px; "
                                "font-family: Monospace; font-size: 14px; font-weight: bold;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    auto *batteryUnit = new QLabel("V", this);
    batteryUnit->setStyleSheet("font-size: 14px; font-weight: bold; color: #555;");
    batteryUnit->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryIcon);
    topBar->addWidget(batteryLabel);
    topBar->addWidget(batteryUnit);

    rightLayout->addLayout(topBar);
    rightLayout->addSpacing(20);

    // Wspólny styl dla okienek z wartościami (dodano font-size: 14px)
    QString valueStyle = "background-color: white; color: black; border: 1px solid #aaa; padding: 2px; font-size: 14px;";

    // pose
    poseTitle = new QLabel(LanguageManager::getInstance().translate("imu_pose_title"), this);
    poseTitle->setStyleSheet("color: #2980b9; font-weight: bold; font-size: 15px;"); 
    poseTitle->setAlignment(Qt::AlignLeft);
    rightLayout->addWidget(poseTitle);

    auto *poseGrid = new QGridLayout();
    poseGrid->setSpacing(5); 

    // x
    auto *xTitle = new QLabel(LanguageManager::getInstance().translate("imu_x_label") + ":", this);
    xTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    xLabel = new QLabel("0.0", this); 
    xLabel->setStyleSheet(valueStyle);
    xLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    xLabel->setMinimumWidth(60);
    
    auto *xUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_meters"), this);

    poseGrid->addWidget(xTitle, 0, 0);
    poseGrid->addWidget(xLabel, 0, 1);
    poseGrid->addWidget(xUnit, 0, 2);

    // y
    auto *yTitle = new QLabel(LanguageManager::getInstance().translate("imu_y_label") + ":", this);
    yTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    yLabel = new QLabel("0.0", this);
    yLabel->setStyleSheet(valueStyle);
    yLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yLabel->setMinimumWidth(60); 
    
    auto *yUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_meters"), this);

    poseGrid->addWidget(yTitle, 1, 0);
    poseGrid->addWidget(yLabel, 1, 1);
    poseGrid->addWidget(yUnit, 1, 2);

    // yaw
    auto *yawTitle = new QLabel(LanguageManager::getInstance().translate("imu_yaw_label") + ":", this);
    yawTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    yawLabel = new QLabel("0.0", this);
    yawLabel->setStyleSheet(valueStyle);
    yawLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yawLabel->setMinimumWidth(60);
    
    auto *yawUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_degrees"), this);

    poseGrid->addWidget(yawTitle, 2, 0);
    poseGrid->addWidget(yawLabel, 2, 1);
    poseGrid->addWidget(yawUnit, 2, 2);

    auto *poseHBox = new QHBoxLayout();
    poseHBox->addStretch();
    poseHBox->addLayout(poseGrid);
    poseHBox->addStretch();
    rightLayout->addLayout(poseHBox);

    rightLayout->addSpacing(20);

    // velocity
    velTitle = new QLabel(LanguageManager::getInstance().translate("imu_velocity_title"), this);
    velTitle->setStyleSheet("color: #c0392b; font-weight: bold; font-size: 15px;");
    velTitle->setAlignment(Qt::AlignLeft); 
    rightLayout->addWidget(velTitle);

    auto *velGrid = new QGridLayout();
    velGrid->setSpacing(5);

    // linear
    auto *linTitle = new QLabel(LanguageManager::getInstance().translate("imu_linear_vel") + ":", this);
    linTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    linearVelLabel = new QLabel("0.0", this);
    linearVelLabel->setStyleSheet(valueStyle);
    linearVelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    linearVelLabel->setMinimumWidth(60); 
    
    auto *linUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_ms"), this);

    velGrid->addWidget(linTitle, 0, 0);
    velGrid->addWidget(linearVelLabel, 0, 1);
    velGrid->addWidget(linUnit, 0, 2);

    // angular
    auto *angTitle = new QLabel(LanguageManager::getInstance().translate("imu_angular_vel") + ":", this);
    angTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    angularVelLabel = new QLabel("0.0", this);
    angularVelLabel->setStyleSheet(valueStyle);
    angularVelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    angularVelLabel->setMinimumWidth(60); 
    
    auto *angUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_degps"), this);

    velGrid->addWidget(angTitle, 1, 0);
    velGrid->addWidget(angularVelLabel, 1, 1);
    velGrid->addWidget(angUnit, 1, 2);

    auto *velHBox = new QHBoxLayout();
    velHBox->addStretch();
    velHBox->addLayout(velGrid);
    velHBox->addStretch();

    rightLayout->addStretch();

    // controlWidget
    controlPad = new ControlPadWidget(this);
    rightLayout->addWidget(controlPad, 0, Qt::AlignRight | Qt::AlignBottom);

    mainHLayout->addLayout(rightLayout, 40);
}

void ImuPage::updateOdom(odomState state) {
    double yaw_deg = state.yaw * (180.0 / M_PI);
    double angular_vel_deg = state.angular_vel * (180.0 / M_PI);

    // Ponieważ UI używa teraz oddzielnych etykiet na tekst i jednostki, 
    // tutaj wstawiamy tylko same wartości liczbowe.
    xLabel->setText(QString::number(state.x, 'f', 1));
    yLabel->setText(QString::number(state.y, 'f', 1));
    yawLabel->setText(QString::number(yaw_deg, 'f', 1));

    linearVelLabel->setText(QString::number(state.linear_vel, 'f', 1));
    angularVelLabel->setText(QString::number(angular_vel_deg, 'f', 1));
}

void ImuPage::updateBattery(double voltage) {
    batteryLabel->setText(QString::number(voltage, 'f', 1));

    float minVolt = 9.0;
    float maxVolt = 12.6;

    double level = (voltage - minVolt) / (maxVolt - minVolt);
    level = qBound(0.0, level, 1.0);

    batteryIcon->setLevel(level);
    QString baseStyle = "background: #222; border-radius: 3px; font-family: Monospace; font-size: 18px; font-weight: bold; ";

    if (level <= 0.2) {
        batteryLabel->setStyleSheet(baseStyle + "color: #FF0000;");
    } else {
        batteryLabel->setStyleSheet(baseStyle + "color: #00FF00;");
    }
}

void ImuPage::updateCameraImage(const QImage &image) {
    if (!image.isNull()) {
        cameraLabel->setPixmap(
            QPixmap::fromImage(image).scaled(
                cameraLabel->size(),
                Qt::KeepAspectRatio, 
                Qt::SmoothTransformation 
            )
        );
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
    cameraLabel->setText(LanguageManager::getInstance().translate("imu_camera_waiting"));
    
    // Update odom labels with fresh translations
    // TODO: Store last odom state to refresh display with new language
}

void ImuPage::updateLabels() {
    // TODO: Implement label updates for dynamic translation
}
