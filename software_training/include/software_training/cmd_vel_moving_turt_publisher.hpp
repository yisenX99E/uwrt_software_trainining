#ifndef CMD_VEL_MOVING_TURT_PUBLISHER_HPP_
#define CMD_VEL_MOVING_TURT_PUBLISHER_HPP_

#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>

#include <rclcpp/rclcpp.hpp>

#include <geometry_msgs/msg/twist.hpp>
#include <software_training/visibility.h>

namespace composition {

class cmd_vel_moving_turt_publisher : public rclcpp::Node {

public:
  SOFTWARE_TRAINING_PUBLIC
  explicit cmd_vel_moving_turt_publisher(const rclcpp::NodeOptions &options);

private:
  // publisher
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher;

  // callback timer
  rclcpp::TimerBase::SharedPtr timer;

  // set quality of service depth - AKA a backlog
  static constexpr unsigned int QUEUE{10};

  static constexpr double LINEAR_SPEED{2.0};
  static constexpr double ANGULAR_SPEED{1.0};
};

} // namespace composition

#endif //  CMD_VEL_MOVING_TURT_PUBLISHER_HPP_
