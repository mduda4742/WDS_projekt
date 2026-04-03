#include "window.hpp"

Window::Window(QWidget *parent) : QMainWindow(parent) {

    // Set up the main window
    setWindowTitle("First Qt App");
    resize(400, 300);

    // Create central widget and layout
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_layout = new QVBoxLayout(m_centralWidget);

    // Create a label and add it to the layout
    m_label = new QLabel("Hello, World!", this);
    m_label->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_label);

    // Create a button and add it to the layout
    m_button = new QPushButton("Close app", this);
    m_layout->addWidget(m_button);
    // Connect the button's clicked signal to the close slot of the main window
    connect(m_button, &QPushButton::clicked, this, &QMainWindow::close);

}
