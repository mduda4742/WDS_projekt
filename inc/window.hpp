#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QGridLayout>
#include <QTabWidget>

/**
 * @class Window
 * @brief Main application window class that inherits from QMainWindow. 
 * This class sets up the GUI with multiple tabs for different functionalities 
 * such as Home, SLAM, and IMU data display. It also includes slots to update 
 * the GUI based on signals emitted from the ROS node.
 */
class Window : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the Window class. Initializes the main window, sets up the 
     * tab widget, and creates the different pages for Home, SLAM, and IMU.
     */
    Window(QWidget *parent = nullptr);
    /**
     * @brief Destructor for the Window class. Currently defaulted, but can be used to clean up any resources if needed in the future.
     */
    virtual ~Window() = default;

public slots:
    /**
     * @brief Slot to update the test data displayed in the GUI.
     * @param msg The new test data message to display.
     */
    void updateTestData(const QString &msg);

private:
    /**
     * @brief Pointer to the QTabWidget that holds the different pages of the GUI. 
     * This allows for easy access and manipulation of the tabs and their contents.
     */
    QTabWidget *tabWidget;

    /**
     * @brief Helper function to create the Home page of the GUI. 
     */
    QWidget *createHomePage();
    /**
     * @brief Helper function to create the SLAM page of the GUI. 
     */
    QWidget *createSlamPage();
    /**
     * @brief Helper function to create the IMU page of the GUI. 
     */
    QWidget *createImuPage();

    /**
     * @brief Pointer to the QLabel used as a placeholder for the SLAM visualization. 
     */
    QLabel *mapPlaceholder;

    QLabel *cameraLabel;
    QLabel *batteryLabel;
    QLabel *xLabel;
    QLabel *yLabel;
    QLabel *yawLabel;

    QLabel *linearVelLabel;
    QLabel *angularVelLabel;
};

#endif
