#include "window.hpp"

Window::Window(QWidget *parent) : QMainWindow(parent) {

    // Set up the main window
    setWindowTitle("Leo Rover Base - Qt GUI");
    resize(800, 600);

    // Create tab widget
    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createHomePage(), "Home");
    tabWidget->addTab(createSlamPage(), "SLAM");
    tabWidget->addTab(createImuPage(), "IMU");
    
    setCentralWidget(tabWidget);
}

QWidget* Window::createHomePage() {
    // Create a simple home page with a welcome message
    auto *page   = new QWidget(this);
    auto *layout = new QVBoxLayout(page);
    auto *title  = new QLabel("<h1>Welcome to the WDS Project Home Page!</h1>", page);
    auto *subtitle = new QLabel("<p>Authors: Radosław Mijał & Mateusz Duda</p>", page);
    title -> setAlignment(Qt::AlignCenter);
    subtitle -> setAlignment(Qt::AlignCenter);

    layout-> addWidget(title);
    layout-> addWidget(subtitle);

    return page;
}

QWidget* Window::createSlamPage() {
    // Create a simple SLAM page
    auto *page   = new QWidget(this);
    auto *layout = new QGridLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // Add map content placeholder on top
    mapPlaceholder = new QLabel("<p>SLAM visualization will go here.</p>", page);
    mapPlaceholder->setStyleSheet("background-color : lightgray; color : white;");
    mapPlaceholder->setAlignment(Qt::AlignCenter);
    layout->addWidget(mapPlaceholder, 0, 0, 1, 2);
    layout->setRowStretch(0, 3); 

    // Add title on bottom left
    auto *title  = new QLabel("<h3>SLAM Page</h3>", page);
    title -> setAlignment(Qt::AlignCenter);
    layout->addWidget(title, 1, 0);
    layout->setRowStretch(1, 1); 

    // Add gamepad-style control layout on bottom right
    auto *controlsWidget = new QWidget(page);
    auto *controlsLayout = new QGridLayout(controlsWidget);
    controlsLayout->setSpacing(3);
    controlsLayout->setContentsMargins(5, 5, 5, 5);
    
    // Set fixed size for square panel
    controlsWidget->setFixedSize(200, 200);
    controlsWidget->setStyleSheet("border: 1px solid black;");

    // Create all 8 directional buttons
    auto *btnUpLeft = new QPushButton("↖", page);
    auto *btnUp = new QPushButton("↑", page);
    auto *btnUpRight = new QPushButton("↗", page);
    
    auto *btnLeft = new QPushButton("←", page);
    auto *btnCenter = new QPushButton("●", page);
    auto *btnRight = new QPushButton("→", page);
    
    auto *btnDownLeft = new QPushButton("↙", page);
    auto *btnDown = new QPushButton("↓", page);
    auto *btnDownRight = new QPushButton("↘", page);

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
    controlsLayout->addWidget(btnUpLeft, 0, 0);
    controlsLayout->addWidget(btnUp, 0, 1);
    controlsLayout->addWidget(btnUpRight, 0, 2);
    
    controlsLayout->addWidget(btnLeft, 1, 0);
    controlsLayout->addWidget(btnCenter, 1, 1);
    controlsLayout->addWidget(btnRight, 1, 2);
    
    controlsLayout->addWidget(btnDownLeft, 2, 0);
    controlsLayout->addWidget(btnDown, 2, 1);
    controlsLayout->addWidget(btnDownRight, 2, 2);

    layout->addWidget(controlsWidget, 1, 1, Qt::AlignRight | Qt::AlignBottom);

    return page;
}

QWidget* Window::createImuPage() {
    auto *page = new QWidget(this);
    auto *mainHLayout = new QHBoxLayout(page);

    // --- LEWA STRONA: Kamera (4:3) ---
    auto *leftLayout = new QVBoxLayout();

    cameraLabel = new QLabel("OCZEKIWANIE NA OBRAZ...", page);
    cameraLabel->setMinimumSize(400, 300); // Startowy rozmiar 4:3

    // Stylistyka "pustego" okna kamery
    cameraLabel->setStyleSheet(
        "background-color: #000000; " // Czarny ekran jak w wyłączonym TV
        "color: #555555; "           // Ciemnoszary tekst
        "border: 3px solid #333333; "
        "border-radius: 10px; "
        "font-weight: bold;"
        );
    cameraLabel->setAlignment(Qt::AlignCenter);

    // Ważne: to sprawi, że etykieta będzie się rozszerzać
    cameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    leftLayout->addWidget(cameraLabel);
    leftLayout->addStretch();

    // Dodajemy lewą stronę z wagą 2 (będzie 2x szersza od prawej)
    mainHLayout->addLayout(leftLayout, 2);


    // --- PRAWA STRONA: Dane (Panel boczny) ---
    auto *rightLayout = new QVBoxLayout();

    // 1. Pasek górny
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel("LEO SYSTEM", page);
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #444;");

    batteryLabel = new QLabel("12.4 V", page);
    batteryLabel->setFixedSize(65, 25);
    batteryLabel->setStyleSheet("background: #222; color: #00FF00; border-radius: 3px; font-family: Monospace;");
    batteryLabel->setAlignment(Qt::AlignCenter);

    topBar->addWidget(title);
    topBar->addStretch();
    topBar->addWidget(batteryLabel);
    rightLayout->addLayout(topBar);

    rightLayout->addSpacing(20);

    // 2. Sekcja POZA
    auto *poseTitle = new QLabel("POSE", page);
    poseTitle->setStyleSheet("color: #2980b9; font-weight: bold;");
    rightLayout->addWidget(poseTitle);

    xLabel   = new QLabel("X:  0.0 m", page);
    yLabel   = new QLabel("Y:  0.0 m", page);
    yawLabel = new QLabel("YAW:  0.0 °", page);

    rightLayout->addWidget(xLabel);
    rightLayout->addWidget(yLabel);
    rightLayout->addWidget(yawLabel);

    rightLayout->addSpacing(20);

    // 3. Sekcja PRĘDKOŚCI
    auto *velTitle = new QLabel("VELOCITY", page);
    velTitle->setStyleSheet("color: #c0392b; font-weight: bold;");
    rightLayout->addWidget(velTitle);

    linearVelLabel  = new QLabel("LIN:  0.0 m/s", page);
    angularVelLabel = new QLabel("ANG:  0.0 °/s", page);

    rightLayout->addWidget(linearVelLabel);
    rightLayout->addWidget(angularVelLabel);

    rightLayout->addStretch();

    // Dodajemy prawą stronę z wagą 1
    mainHLayout->addLayout(rightLayout, 1);

    return page;
}

void Window::updateTestData(const QString &msg) {
    // Send msg
    mapPlaceholder->setText("Ros: " + msg);
}
