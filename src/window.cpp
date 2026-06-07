#include "window.hpp"
#include "ros_node.hpp"
#include "language_manager.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


Window::Window(QWidget *parent) : QMainWindow(parent), homeTitle(nullptr), homeSubtitle(nullptr), homeInfoLabel(nullptr) {
    setWindowTitle(LanguageManager::getInstance().translate("window_title"));
    showFullScreen();

    // Apply global dark theme with white, much larger text
    setStyleSheet("QWidget { background-color: #222222; color: white; font-size: 28px; }"
                  "QTabWidget::pane { border: 1px solid #444; }"
                  "QTabBar::tab { background: #333; padding: 12px 24px; }"
                  "QTabBar::tab:selected { background: #555; }");

    // Create window control buttons for the top right corner
    auto *cornerWidget = new QWidget(this);
    auto *cornerLayout = new QHBoxLayout(cornerWidget);
    cornerLayout->setContentsMargins(0, 0, 0, 0);
    
    auto *minBtn = new QPushButton("_", cornerWidget);
    auto *adjBtn = new QPushButton("□", cornerWidget);
    auto *closeBtn = new QPushButton("✕", cornerWidget);
    
    connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(adjBtn, &QPushButton::clicked, [this]() {
        if (this->isFullScreen()) this->showNormal();
        else this->showFullScreen();
    });
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);
    
    cornerLayout->addWidget(minBtn);
    cornerLayout->addWidget(adjBtn);
    cornerLayout->addWidget(closeBtn);

    // Create tab widget to hold all pages
    tabWidget = new QTabWidget(this);
    tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);
    tabWidget->addTab(createHomePage(), LanguageManager::getInstance().translate("tab_home"));
    
    // Create SLAM page for LIDAR visualization and movement controls
    slamPage = new SlamPage(this);
    tabWidget->addTab(slamPage, LanguageManager::getInstance().translate("tab_slam"));
    
    // Create IMU page for sensor data display
    imuPage = new ImuPage(this);
    tabWidget->addTab(imuPage, LanguageManager::getInstance().translate("tab_imu"));
    
    setCentralWidget(tabWidget);

    // Connect language changes to update UI
    connect(&LanguageManager::getInstance(), &LanguageManager::languageChanged,
            this, &Window::refreshLanguage);
}

QWidget* Window::createHomePage() {
    auto *page = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(page);
    
    // Top section: Title and subtitle
    auto *topLayout = new QVBoxLayout();
    homeTitle = new QLabel("<h1>" + LanguageManager::getInstance().translate("home_welcome_title") + "</h1>", page);
    homeTitle->setAlignment(Qt::AlignCenter);
    
    homeSubtitle = new QLabel("<p>" + LanguageManager::getInstance().translate("home_subtitle") + "</p>", page);
    homeSubtitle->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(homeTitle);
    topLayout->addWidget(homeSubtitle);
    
    mainLayout->addLayout(topLayout);
    
    // Middle section: Info text area
    homeInfoLabel = new QLabel(LanguageManager::getInstance().translate("home_info_text"), page);
    homeInfoLabel->setAlignment(Qt::AlignCenter);
    homeInfoLabel->setStyleSheet("padding: 40px;");
    mainLayout->addWidget(homeInfoLabel, 1);  // Stretch to fill available space
    
    // Bottom section: Language selector (bottom right corner)
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    
    auto *langLabel = new QLabel(LanguageManager::getInstance().translate("home_language_label"), page);
    
    auto *englishBtn = new QPushButton(LanguageManager::getInstance().translate("home_english_button"), page);
    englishBtn->setMaximumWidth(150);
    
    auto *polishBtn = new QPushButton(LanguageManager::getInstance().translate("home_polish_button"), page);
    polishBtn->setMaximumWidth(150);
    
    connect(englishBtn, &QPushButton::clicked, [this]() {
        LanguageManager::getInstance().setLanguage(Language::English);
    });
    
    connect(polishBtn, &QPushButton::clicked, [this]() {
        LanguageManager::getInstance().setLanguage(Language::Polish);
    });
    
    bottomLayout->addWidget(langLabel);
    bottomLayout->addWidget(englishBtn);
    bottomLayout->addWidget(polishBtn);
    bottomLayout->addSpacing(10);  // Small margin from right edge
    
    mainLayout->addLayout(bottomLayout);

    return page;
}

void Window::updateOdomData(odomState state) {
    imuPage->updateOdom(state);
}

void Window::updateBatteryData(double voltage) {
    imuPage->updateBattery(voltage);
}

void Window::updateCameraImage(const QImage &image) {
    if (imuPage) {
        imuPage->updateCameraImage(image);
    }
}

void Window::updateLaserData(const std::vector<float> &ranges, 
                            float angle_min, float angle_max, float angle_increment) {
    slamPage->updateLaserData(ranges, angle_min, angle_max, angle_increment);
    if (imuPage) {
        imuPage->updateLaserScan(ranges, angle_min, angle_max, angle_increment);
    }
}

void Window::updatePathData(const std::vector<double> &path_x, const std::vector<double> &path_y) {
    slamPage->updatePathData(path_x, path_y);
}

void Window::updateRobotPose(double x, double y, double theta) {
    slamPage->updateRobotPose(x, y, theta);
}

void Window::setRosNode(RosNode *node) {
    slamPage->setRosNode(node);
    imuPage->setRosNode(node);
}

void Window::refreshLanguage() {
    // Update window title
    setWindowTitle(LanguageManager::getInstance().translate("window_title"));
    
    // Update tab names
    tabWidget->setTabText(0, LanguageManager::getInstance().translate("tab_home"));
    tabWidget->setTabText(1, LanguageManager::getInstance().translate("tab_slam"));
    tabWidget->setTabText(2, LanguageManager::getInstance().translate("tab_imu"));
    
    // Update home page text
    if (homeTitle) {
        homeTitle->setText("<h1>" + LanguageManager::getInstance().translate("home_welcome_title") + "</h1>");
    }
    if (homeSubtitle) {
        homeSubtitle->setText("<p>" + LanguageManager::getInstance().translate("home_subtitle") + "</p>");
    }
    if (homeInfoLabel) {
        homeInfoLabel->setText(LanguageManager::getInstance().translate("home_info_text"));
    }
    
    // Notify child pages to refresh their text
    if (slamPage) {
        slamPage->refreshLanguage();
    }
    if (imuPage) {
        imuPage->refreshLanguage();
    }
}
