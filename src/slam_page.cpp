#include "slam_page.hpp"

SlamPage::SlamPage(QWidget *parent) : QWidget(parent) {
    auto *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map content placeholder on top (takes 3/4 of the screen)
    mapPlaceholder = new QLabel("<p>SLAM visualization will go here.</p>", this);
    mapPlaceholder->setStyleSheet("background-color : lightgray; color : white;");
    mapPlaceholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapPlaceholder, 0, 0, 1, 2);
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

void SlamPage::updateMapData(const QString &msg) {
    mapPlaceholder->setText("Ros: " + msg);
}
