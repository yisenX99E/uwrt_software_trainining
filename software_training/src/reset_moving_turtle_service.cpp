#include <chrono>
#include <software_training/reset_moving_turtle_service.hpp>

using namespace std::placeholders;
using namespace std::chrono_literals;

namespace composition {

reset_moving_turtle_service::reset_moving_turtle_service(
    const rclcpp::NodeOptions &options)
    : Node("reset_moving_turtle_service", options) {

  // The client response must be processed while the service callback waits for
  // it, so keep them in separate callback groups and use a multithreaded
  // component container.
  client_callbacks =
      this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
  service_callbacks =
      this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  this->client = this->create_client<turtlesim_msgs::srv::TeleportAbsolute>(
      "/moving_turtle/teleport_absolute", rclcpp::ServicesQoS(),
      client_callbacks);

  // create service
  this->service = this->create_service<software_training::srv::Software>(
      "/reset_moving_turtle",
      std::bind(&reset_moving_turtle_service::service_callback, this, _1, _2),
      rclcpp::ServicesQoS(), service_callbacks);
}

void reset_moving_turtle_service::service_callback(
    const std::shared_ptr<software_training::srv::Software::Request> request,
    std::shared_ptr<software_training::srv::Software::Response> response) {

  (void)request; // request is not needed

  RCLCPP_INFO(this->get_logger(), "Starting ...");

  // make client call to reset turtle
  if (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(this->get_logger(), "System Aborted");
      response->success = false;
      return;
    }
    RCLCPP_INFO(this->get_logger(), "Service is not available! Exit!");
    response->success = false;
    return;
  }

  auto client_request =
      std::make_shared<turtlesim_msgs::srv::TeleportAbsolute::Request>();

  // fill request data
  client_request->x = reset_moving_turtle_service::reset_coordinates::x;
  client_request->y = reset_moving_turtle_service::reset_coordinates::y;
  client_request->theta = reset_moving_turtle_service::reset_coordinates::theta;

  auto result = client->async_send_request(client_request);
  if (result.wait_for(2s) != std::future_status::ready) {
    RCLCPP_ERROR(this->get_logger(), "Timed out while resetting moving_turtle");
    response->success = false;
    return;
  }

  try {
    (void)result.get();
    response->success = true;
    RCLCPP_INFO(this->get_logger(), "moving_turtle reset to (25, 10)");
  } catch (const std::exception &error) {
    RCLCPP_ERROR(this->get_logger(), "Reset failed: %s", error.what());
    response->success = false;
  }
}

} // namespace composition

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(composition::reset_moving_turtle_service)
