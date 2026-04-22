#ifndef ROS_NODE_HPP
#define ROS_NODE_HPP

#include <rclcpp/rclcpp.hpp>
#include <QObject>
#include <std_msgs/msg/string.hpp> // testing
#include <geometry_msgs/msg/vector3_stamped.hpp>


/**
 * @class RosNode
 * @brief Main ROS 2 node class that integrates with Qt. This node subscribes to a test 
 * topic and emits a Qt signal when new data is received, allowing the GUI to update accordingly.
 */
class RosNode : public QObject, public rclcpp::Node {
    Q_OBJECT
public:
    /**
     * @brief Constructor for the RosNode class. Initializes the ROS node with a name and sets up 
     * the subscription to the test topic. 
     */
    RosNode();
    /**
     * @brief Destructor for the RosNode class. Currently defaulted, but can be used to clean up any resources if needed in the future.
     */
    virtual ~RosNode() = default;
    
signals:
   /**
    * @brief Signal emitted when new data is received from the ROS topic. The message content 
    * is passed as a QString to be easily used in the Qt GUI.
    * @param msg The message content received from the ROS topic, converted to QString format.
    */
    void testDataReceived(const QString &msg); // testing

    void rpyReceived(double roll, double pitch, double yaw);

private:
    /**
     * @brief Callback function for the test topic subscription. This function is called whenever 
     * a new message is received on the "test_topic". It emits the testDataReceived signal with the message content.
     */
    void testCallback(const std_msgs::msg::String::SharedPtr msg); // testing

    /**
     * @brief ROS 2 subscription for the test topic. This subscription listens for messages on the "test_topic" 
     * and triggers the testCallback function when new messages arrive. This is used for testing the integration between ROS and Qt.
     */
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr test_sub_; // testing

    void rpyCallback(const geometry_msgs::msg::Vector3Stamped::SharedPtr rpy);
    rclcpp::Subscription<geometry_msgs::msg::Vector3Stamped>::SharedPtr rpy_sub_;
};

#endif 
