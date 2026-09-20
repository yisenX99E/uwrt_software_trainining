#include <software_training/turtle_neutralize.hpp>

using namespace std::chrono_literals;

namespace composition {

turtle_service_request_node::turtle_service_request_node(
    const rclcpp::NodeOptions &options)
    : Node("turtle_service_request_node", options) {
  // create client
  client = this->create_client<std_srvs::srv::Empty>("/reset");

  // create callback
  timer = this->create_wall_timer(
      500ms, std::bind(&turtle_service_request_node::reset_simulator, this));
}

void turtle_service_request_node::reset_simulator() {

  // check if service exists
  if (!client->wait_for_service(2s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(),
                   "Interrupted while waiting for service. Exiting!");
      return;
    }
    RCLCPP_INFO(this->get_logger(), "Service not available after waiting");
    return;
  }

  auto request = std::make_shared<std_srvs::srv::Empty::Request>();
  auto callback =
      [this](rclcpp::Client<std_srvs::srv::Empty>::SharedFuture response) {
    (void)response;
    RCLCPP_INFO(this->get_logger(),
                "Simulator cleared and turtle1 restored");
  };
  auto result = client->async_send_request(request, callback);
  (void)result;

  // This setup operation is intentionally one-shot. Shutting down rclcpp here
  // would terminate every component in the shared container.
  timer->cancel();
}

} // namespace composition

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(composition::turtle_service_request_node)
