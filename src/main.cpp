#include <QApplication>
#include "window.hpp"
#include "ros_node.hpp"
#include <thread>

int main(int argc, char *argv[]) {
    // Initialize ROS 2
    rclcpp::init(argc, argv);
    
    // Initialize Qt application
    QApplication app(argc, argv);

    // Create ROS 2 node as shared_ptr
    auto ros_node = std::make_shared<RosNode>();
    Window window;

    // Connect ROS node signals to GUI slots
    QObject::connect(ros_node.get(), &RosNode::testDataReceived, 
    &window, &Window::updateMapData);

    // Start ROS spinning in a separate thread (capture by value)
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