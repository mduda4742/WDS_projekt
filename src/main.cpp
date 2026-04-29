#include <QApplication>
#include "window.hpp"
#include "ros_node.hpp"
#include "lidar_sim_node.hpp"
#include <thread>

int main(int argc, char *argv[]) {
    // Initialize ROS 2
    rclcpp::init(argc, argv);
    
    // Initialize Qt application
    QApplication app(argc, argv);

    // Create ROS 2 node
    auto ros_node = std::make_shared<RosNode>();
    
    // Create main window
    Window window;
    window.setRosNode(ros_node.get());

    // Connect ROS node signals to GUI slots
    QObject::connect(ros_node.get(), &RosNode::rpyReceived, 
                     &window, [&window](double yaw) {
                         // Can handle yaw data here if needed
                     });
    
    QObject::connect(ros_node.get(), &RosNode::laserScanReceived,
                     &window, &Window::updateLaserData);

    QObject::connect(ros_node.get(), &RosNode::yawReceived,
                     &window, &Window::updateYawData);

    QObject::connect(ros_node.get(), &RosNode::batteryReceived,
                     &window, &Window::updateBatteryData);

    // Start ROS spinning in a separate thread
    std::thread ros_thread([ros_node]() {
        rclcpp::spin(ros_node);
    });
    ros_thread.detach();  // Detach thread so it runs independently

    std::thread lidar_thread([]() {
        auto lidar_simulator = std::make_shared<LidarSimulator>();
        rclcpp::spin(lidar_simulator);
    });
    lidar_thread.detach();  

    // Show the GUI
    window.show();
    int result = app.exec();
    
    // Shutdown ROS 2 when Qt app exits
    rclcpp::shutdown();
    
    return result;
}