#ifndef TURTLE_PUBLISHER_HPP_
#define TURTLE_PUBLISHER_HPP_

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include <software_training/msg/software.hpp>
#include <software_training/visibility.h>
#include <turtlesim_msgs/msg/pose.hpp>

namespace composition {

class turtle_publisher : public rclcpp::Node {
public:
  SOFTWARE_TRAINING_PUBLIC
  explicit turtle_publisher(const rclcpp::NodeOptions &options);

private:
  // position subscribers
  rclcpp::Subscription<turtlesim_msgs::msg::Pose>::SharedPtr stationary_turt_sub;
  rclcpp::Subscription<turtlesim_msgs::msg::Pose>::SharedPtr moving_turt_sub;

  // turtle publisher with custom message
  rclcpp::Publisher<software_training::msg::Software>::SharedPtr publisher;

  // timer for publisher callback
  rclcpp::TimerBase::SharedPtr timer;

  // callback groups - really just threads to run callbacks
  rclcpp::CallbackGroup::SharedPtr callbacks;

  float x_stationary_turt{0.0f};
  float y_stationary_turt{0.0f};

  float x_moving_turt{0.0f};
  float y_moving_turt{0.0f};

  bool stationary_pose_received{false};
  bool moving_pose_received{false};

  static const unsigned int QUEUE{10};
};
} // namespace composition

#endif // TURTLE_PUBLISHER_HPP_
