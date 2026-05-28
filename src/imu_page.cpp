#include "imu_page.hpp"
#include "battery_widget.hpp"
#include "control_pad_widget.hpp"
#include "gl_rover_widget.hpp"
#include <cmath>
#include <QVBoxLayout>
#include "ros_node.hpp"
/*
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

    batteryIcon = new BatteryWidget(this);

    batteryLabel = new QLabel("12.4 V", this);
    batteryLabel->setFixedSize(65, 25);
    batteryLabel->setStyleSheet("background: #222; color: #00FF00; border-radius: 3px; font-family: Monospace;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryIcon);
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

    controlPad = new ControlPadWidget(this);
    rightLayout->addWidget(controlPad, 0, Qt::AlignRight | Qt::AlignBottom);

    mainHLayout->addLayout(rightLayout, 1);
}
*/

ImuPage::ImuPage(QWidget *parent) : QWidget(parent) {
    auto *mainHLayout = new QHBoxLayout(this);

    auto *leftLayout = new QVBoxLayout();

    // --- ROZWIĄZANIE BUGU: Tworzymy widżet OpenGL bez rodzica (this) w konstruktorze ---
    // Layout sam przejmie nad nim kontrolę i prawidłowo osadzi go w oknie na Linuksie
    auto *glWidget = new GLRoverWidget();
    glWidget->setMinimumSize(400, 300);
    glWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Wrzucamy OpenGL jako jedyny, główny element lewej strony interfejsu
    leftLayout->addWidget(glWidget);

    // --- BEZPIECZEŃSTWO ---
    // Kamera zostaje całkowicie usunięta z widoku i layoutu. 
    // Inicjalizujemy ją tylko w pamięci i od razu chowamy, żeby ewentualne 
    // callbacki z ROS-a (np. cameraLabel->setPixmap) nie wysypały całego programu.
    cameraLabel = new QLabel(this);
    cameraLabel->hide();

    mainHLayout->addLayout(leftLayout, 2);

    // --- PRAWA STRONA (Nienaruszona, wszystkie nazwy zmiennych bez zmian) ---
    auto *rightLayout = new QVBoxLayout();

    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel("LEO SYSTEM", this);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #444;");

    batteryIcon = new BatteryWidget(this);

    batteryLabel = new QLabel("12.4 V", this);
    batteryLabel->setFixedSize(65, 25);
    batteryLabel->setStyleSheet("background: #222; color: #00FF00; border-radius: 3px; font-family: Monospace;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryIcon);
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

    controlPad = new ControlPadWidget(this);
    rightLayout->addWidget(controlPad, 0, Qt::AlignRight | Qt::AlignBottom);

    mainHLayout->addLayout(rightLayout, 1);
}

void ImuPage::updateOdom(odomState state) {
    double yaw_deg = state.yaw * (180.0 / M_PI);
    double angular_vel_deg = state.angular_vel * (180.0 / M_PI);

    xLabel->setText(QString("X:  %1 m").arg(state.x, 0, 'f', 1));
    yLabel->setText(QString("Y:  %1 m").arg(state.y, 0, 'f', 1));
    yawLabel->setText(QString("YAW:  %1 °").arg(yaw_deg, 0, 'f', 1));

    linearVelLabel->setText(QString("LIN:  %1 m/s").arg(state.linear_vel, 0, 'f', 1));
    angularVelLabel->setText(QString("ANG:  %1 °/s").arg(angular_vel_deg, 0, 'f', 1));
}

void ImuPage::updateBattery(double voltage) {
    batteryLabel->setText(QString("%1 V").arg(voltage, 0, 'f', 1));

    float minVolt = 9.0;
    float maxVolt = 12.6;

    double level = (voltage - minVolt) / (maxVolt - minVolt);

    level = qBound(0.0, level, 1.0);

    batteryIcon->setLevel(level);

    if (level <= 0.2) {
        batteryLabel->setStyleSheet("color: #FF0000; font-family: Monospace; font-weight: bold;");
    } else {
        batteryLabel->setStyleSheet("color: #00FF00; font-family: Monospace; font-weight: bold;");
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
