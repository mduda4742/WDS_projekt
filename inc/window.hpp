#ifndef WINDOW
#define WINDOW

#include <QMainWindow>
#include <QLabel>
#include <QTabWidget>
#include <vector>
#include "slam_page.hpp"
#include "imu_page.hpp"

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
     * @brief Slot to update the SLAM map data displayed in the GUI.
     * @param msg The new map data message to display.
     */
    void updateMapData(const QString &msg);

    void updateYawData(double yaw);

    void updateBatteryData(double voltage);

    /**
     * @brief Slot to update the IMU accel data displayed in the GUI.
     * @param msg The new accel data message to display.
     */
    // void updateAccelData(const QString &msg);

    /**
     * @brief Slot to update the IMU gyro data displayed in the GUI.
     * @param msg The new gyro data message to display.
     */
    // void updateGyroData(const QString &msg);


    /**
     * @brief Slot to update the SLAM map with LIDAR scan data.
     */
    void updateLaserData(const std::vector<float> &ranges, 
                        float angle_min, float angle_max, float angle_increment);

private:
    /**
     * @brief Pointer to the QTabWidget that holds the different pages of the GUI. 
     * This allows for easy access and manipulation of the tabs and their contents.
     */
    QTabWidget *tabWidget;

    /**
     * @brief Pointer to the SLAM page.
     */
    SlamPage *slamPage;

    /**
     * @brief Pointer to the IMU page.
     */
    ImuPage *imuPage;

    /**
     * @brief Helper function to create the Home page of the GUI. 
     */
    QWidget *createHomePage();
};

#endif
