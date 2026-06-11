#include <QApplication>
#include "window.hpp"
#include "ros_node.hpp"
#include <thread>
#include "odomState.hpp"

// xdg-open html/index.html 
// xhost +local:root
// docker compose run --rm dev_environment bash


int main(int argc, char *argv[]) {
    // Initialize ROS 2
    rclcpp::init(argc, argv);
    
    // Initialize Qt application
    QApplication app(argc, argv);

    // Create ROS 2 node
    qRegisterMetaType<odomState>("odomState");

    // Create ROS 2 node as shared_ptr
    auto ros_node = std::make_shared<RosNode>();
    
    // Create main window
    Window window;
    window.setRosNode(ros_node.get());

    // Connect ROS node signals to GUI slots
    QObject::connect(ros_node.get(), &RosNode::laserScanReceived,
                     &window, &Window::updateLaserData);

    QObject::connect(ros_node.get(), &RosNode::pathReceived,
                     &window, &Window::updatePathData);

    QObject::connect(ros_node.get(), &RosNode::odomReceived,
                     &window, &Window::updateOdomData);

    QObject::connect(ros_node.get(), &RosNode::batteryReceived,
                     &window, &Window::updateBatteryData);

    QObject::connect(ros_node.get(), &RosNode::imageReceived,
                     &window, &Window::updateCameraImage);

    // Start ROS spinning in a separate thread
    std::thread ros_thread([ros_node]() {
        rclcpp::spin(ros_node);
    });
    ros_thread.detach();  // Detach thread so it runs independently

    // Show the GUI
    window.show();
    int result = app.exec();
    
    // Shutdown ROS 2 when Qt app exits
    rclcpp::shutdown();
    
    return result;
}