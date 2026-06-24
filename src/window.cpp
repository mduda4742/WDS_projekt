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

    setStyleSheet("QWidget { background-color: #222222; color: white; font-size: 15px; }"
                  "QTabWidget::pane { border: 1px solid #444; }"
                  "QTabBar::tab { background: #333; padding: 12px 24px; }"
                  "QTabBar::tab:selected { background: #555; }");

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

    tabWidget = new QTabWidget(this);
    tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);
    tabWidget->addTab(createHomePage(), LanguageManager::getInstance().translate("tab_home"));
    
    slamPage = new SlamPage(this);
    tabWidget->addTab(slamPage, LanguageManager::getInstance().translate("tab_slam"));
    
    imuPage = new ImuPage(this);
    tabWidget->addTab(imuPage, LanguageManager::getInstance().translate("tab_imu"));
    
    setCentralWidget(tabWidget);

    connect(&LanguageManager::getInstance(), &LanguageManager::languageChanged,
            this, &Window::refreshLanguage);
}

QWidget* Window::createHomePage() {
    auto *page = new QWidget(this);
    
    auto *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(40, 40, 40, 30); 
    
    auto *contentLayout = new QHBoxLayout();
    
    auto *textLayout = new QVBoxLayout();
    textLayout->setAlignment(Qt::AlignTop);
    textLayout->setSpacing(25);
    
    homeTitle = new QLabel("<h1>" + LanguageManager::getInstance().translate("home_welcome_title") + "</h1>", page);
    homeTitle->setWordWrap(true); 
    
    homeSubtitle = new QLabel("<p>" + LanguageManager::getInstance().translate("home_subtitle") + "</p>", page);
    homeSubtitle->setStyleSheet("color: #aaaaaa; font-size: 16px;");
    homeSubtitle->setWordWrap(true);

    homeInfoLabel = new QLabel(LanguageManager::getInstance().translate("home_info_text"), page);
    homeInfoLabel->setWordWrap(true);
    homeInfoLabel->setStyleSheet("background-color: #2a2a2a; padding: 25px; border-radius: 10px; line-height: 1.5;");
    
    textLayout->addWidget(homeTitle);
    textLayout->addWidget(homeSubtitle);
    textLayout->addSpacing(15); 
    textLayout->addWidget(homeInfoLabel);
    textLayout->addStretch();
    
    rovWidget = new GLRoverWidget(page); 
    rovWidget->setMinimumSize(450, 450);
    rovWidget->setStyleSheet("border: 1px solid #444; border-radius: 10px; background-color: #1a1a1a;");
    
    contentLayout->addLayout(textLayout, 1);
    contentLayout->addSpacing(50);
    contentLayout->addWidget(rovWidget, 1);
    
    mainLayout->addLayout(contentLayout, 1); 
    
    auto *bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch(); 
    
    langLabel = new QLabel(LanguageManager::getInstance().translate("home_language_label"), page);
    
    englishBtn = new QPushButton(LanguageManager::getInstance().translate("home_english_button"), page);
    englishBtn->setFixedSize(120, 40); 
    englishBtn->setStyleSheet("background-color: #333; border-radius: 5px;");
    
    polishBtn = new QPushButton(LanguageManager::getInstance().translate("home_polish_button"), page);
    polishBtn->setFixedSize(120, 40);
    polishBtn->setStyleSheet("background-color: #333; border-radius: 5px;");
    
    connect(englishBtn, &QPushButton::clicked, [this]() {
        LanguageManager::getInstance().setLanguage(Language::English);
    });
    
    connect(polishBtn, &QPushButton::clicked, [this]() {
        LanguageManager::getInstance().setLanguage(Language::Polish);
    });
    
    bottomLayout->addWidget(langLabel);
    bottomLayout->addSpacing(15); 
    bottomLayout->addWidget(englishBtn);
    bottomLayout->addWidget(polishBtn);
    
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
    setWindowTitle(LanguageManager::getInstance().translate("window_title"));
    
    tabWidget->setTabText(0, LanguageManager::getInstance().translate("tab_home"));
    tabWidget->setTabText(1, LanguageManager::getInstance().translate("tab_slam"));
    tabWidget->setTabText(2, LanguageManager::getInstance().translate("tab_imu"));
    
    if (homeTitle) {
        homeTitle->setText("<h1>" + LanguageManager::getInstance().translate("home_welcome_title") + "</h1>");
    }
    if (homeSubtitle) {
        homeSubtitle->setText("<p>" + LanguageManager::getInstance().translate("home_subtitle") + "</p>");
    }
    if (homeInfoLabel) {
        homeInfoLabel->setText(LanguageManager::getInstance().translate("home_info_text"));
    }
    if (langLabel) {
        langLabel->setText(LanguageManager::getInstance().translate("home_language_label"));
    }
    if (englishBtn) {
        englishBtn->setText(LanguageManager::getInstance().translate("home_english_button"));
    }
    if (polishBtn) {
        polishBtn->setText(LanguageManager::getInstance().translate("home_polish_button"));
    }
    
    if (slamPage) {
        slamPage->refreshLanguage();
    }
    if (imuPage) {
        imuPage->refreshLanguage();
    }
}
