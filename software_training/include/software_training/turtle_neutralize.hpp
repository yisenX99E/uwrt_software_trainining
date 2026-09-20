#ifndef TURTLE_SERVICE_REQUEST_NODE_HPP_
#define TURTLE_SERVICE_REQUEST_NODE_HPP_

#include <cstdlib>
#include <memory>
#include <rclcpp/rclcpp.hpp>

#include <software_training/visibility.h>
#include <std_srvs/srv/empty.hpp>

namespace composition {

class turtle_service_request_node : public rclcpp::Node {
public:
  SOFTWARE_TRAINING_PUBLIC
  explicit turtle_service_request_node(const rclcpp::NodeOptions &options);

private:
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr client;
  rclcpp::TimerBase::SharedPtr timer;

  SOFTWARE_TRAINING_LOCAL
  void reset_simulator();
};

} // namespace composition
#endif // TURTLE_SERVICE_REQUEST_NODE_HPP_
