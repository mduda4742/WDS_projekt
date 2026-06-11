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
    
    /*
    // glWidget
    auto *glWidget = new GLRoverWidget();
    glWidget->setMinimumSize(400, 400);
    glWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    leftLayout->addWidget(glWidget);

    // Przetłumaczony ukryty label kamery
    cameraLabel = new QLabel(LanguageManager::getInstance().translate("imu_camera_waiting"), this);
    cameraLabel->hide();
    */
    
    
    // cameraWidget
    cameraLabel = new QLabel("OCZEKIWANIE NA OBRAZ...", this);
    cameraLabel->setMinimumSize(400, 300);

    cameraLabel->setAlignment(Qt::AlignCenter);

    cameraLabel->setStyleSheet(
        "background-color: #000000; "
        "color: #555555; "
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

    // Tytuł - przypisany do zmiennej klasowej (mainTitleLabel) zamiast lokalnej
    mainTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_title"), this);
    mainTitleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #EEEEEE;");

    // battery
    batteryIcon = new BatteryWidget(this);

    batteryLabel = new QLabel("12.4", this);
    batteryLabel->setFixedSize(50, 25); 
    batteryLabel->setStyleSheet("background: #333; color: #00FF00; border: 1px solid #666; border-radius: 3px; "
                                "font-family: Monospace; font-size: 15px; font-weight: bold;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    auto *batteryUnit = new QLabel("V", this);
    batteryUnit->setStyleSheet("font-size: 15px; font-weight: bold; color: #CCCCCC;");
    batteryUnit->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    topBar->addWidget(mainTitleLabel); // Używamy nowej zmiennej
    topBar->addStretch();
    topBar->addWidget(batteryIcon);
    topBar->addWidget(batteryLabel);
    topBar->addWidget(batteryUnit);

    rightLayout->addLayout(topBar);
    rightLayout->addSpacing(20);

    // Wspólny styl dla okienek z wartościami
    QString valueStyle = "background-color: white; color: black; border: 1px solid #aaa; padding: 2px;";

    // pose 
    poseTitle = new QLabel(LanguageManager::getInstance().translate("imu_pose_title"), this);
    poseTitle->setStyleSheet("color: #2980b9; font-weight: bold; font-size: 18px;"); 
    poseTitle->setAlignment(Qt::AlignLeft);
    rightLayout->addWidget(poseTitle);

    auto *poseGrid = new QGridLayout();
    poseGrid->setSpacing(5); 

    // x (zmienna klasowa xTitleLabel)
    xTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_x_label") + ":", this);
    xTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    xLabel = new QLabel("0.0", this); 
    xLabel->setStyleSheet(valueStyle);
    xLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    xLabel->setFixedWidth(80); 
    
    auto *xUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_meters"), this);

    poseGrid->addWidget(xTitleLabel, 0, 0); // Używamy nowej zmiennej
    poseGrid->addWidget(xLabel, 0, 1);
    poseGrid->addWidget(xUnit, 0, 2);

    // y (zmienna klasowa yTitleLabel)
    yTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_y_label") + ":", this);
    yTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    yLabel = new QLabel("0.0", this);
    yLabel->setStyleSheet(valueStyle);
    yLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yLabel->setFixedWidth(80); 
    
    auto *yUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_meters"), this);

    poseGrid->addWidget(yTitleLabel, 1, 0); // Używamy nowej zmiennej
    poseGrid->addWidget(yLabel, 1, 1);
    poseGrid->addWidget(yUnit, 1, 2);

    // yaw (zmienna klasowa yawTitleLabel)
    yawTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_yaw_label") + ":", this);
    yawTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    yawLabel = new QLabel("0.0", this);
    yawLabel->setStyleSheet(valueStyle);
    yawLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yawLabel->setFixedWidth(80);
    
    auto *yawUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_degrees"), this);

    poseGrid->addWidget(yawTitleLabel, 2, 0); // Używamy nowej zmiennej
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
    velTitle->setStyleSheet("color: #c0392b; font-weight: bold; font-size: 18px;");
    velTitle->setAlignment(Qt::AlignLeft); 
    rightLayout->addWidget(velTitle);

    auto *velGrid = new QGridLayout();
    velGrid->setSpacing(5);

    // linear (zmienna klasowa linTitleLabel)
    linTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_linear_vel") + ":", this);
    linTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    linearVelLabel = new QLabel("0.0", this);
    linearVelLabel->setStyleSheet(valueStyle);
    linearVelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    linearVelLabel->setFixedWidth(80); 
    
    auto *linUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_ms"), this);

    velGrid->addWidget(linTitleLabel, 0, 0); // Używamy nowej zmiennej
    velGrid->addWidget(linearVelLabel, 0, 1);
    velGrid->addWidget(linUnit, 0, 2);

    // angular (zmienna klasowa angTitleLabel)
    angTitleLabel = new QLabel(LanguageManager::getInstance().translate("imu_angular_vel") + ":", this);
    angTitleLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    angularVelLabel = new QLabel("0.0", this);
    angularVelLabel->setStyleSheet(valueStyle);
    angularVelLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    angularVelLabel->setFixedWidth(80); 
    
    auto *angUnit = new QLabel(LanguageManager::getInstance().translate("imu_unit_degps"), this);

    velGrid->addWidget(angTitleLabel, 1, 0); // Używamy nowej zmiennej
    velGrid->addWidget(angularVelLabel, 1, 1);
    velGrid->addWidget(angUnit, 1, 2);

    auto *velHBox = new QHBoxLayout();
    velHBox->addStretch();
    velHBox->addLayout(velGrid);
    velHBox->addStretch();
    rightLayout->addLayout(velHBox);

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
    // 1. Główne tytuły sekcji i obraz z kamery
    poseTitle->setText(LanguageManager::getInstance().translate("imu_pose_title"));
    velTitle->setText(LanguageManager::getInstance().translate("imu_velocity_title"));
    cameraLabel->setText(LanguageManager::getInstance().translate("imu_camera_waiting"));
    mainTitleLabel->setText(LanguageManager::getInstance().translate("imu_title"));
    
    // 2. Etykiety osi i prędkości (dodajemy też dwukropek z powrotem, żeby ładnie wyglądało)
    xTitleLabel->setText(LanguageManager::getInstance().translate("imu_x_label") + ":");
    yTitleLabel->setText(LanguageManager::getInstance().translate("imu_y_label") + ":");
    yawTitleLabel->setText(LanguageManager::getInstance().translate("imu_yaw_label") + ":");
    
    linTitleLabel->setText(LanguageManager::getInstance().translate("imu_linear_vel") + ":");
    angTitleLabel->setText(LanguageManager::getInstance().translate("imu_angular_vel") + ":");

    // TODO: Jeśli chcesz w przyszłości odświeżać inne dane (np. jednostki), 
    // trzeba będzie dodać je do pliku .hpp w podobny sposób.
}

void ImuPage::updateLabels() {
    // TODO: Implement label updates for dynamic translation
}
