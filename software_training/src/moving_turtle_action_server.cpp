#include <algorithm>
#include <cmath>
#include <memory>
#include <software_training/moving_turtle_action_server.hpp>

using namespace std::chrono_literals;
using namespace std::placeholders;

namespace {

template <typename T>
T clamp_value(const T &value, const T &low, const T &high) {
  return value < low ? low : (value > high ? high : value);
}

} // namespace

namespace composition {

moving_turtle_action_server::moving_turtle_action_server(
    const rclcpp::NodeOptions &options)
    : Node("moving_turtle_action_server", options) {

  this->publisher = this->create_publisher<geometry_msgs::msg::Twist>(
      "/moving_turtle/cmd_vel", rclcpp::QoS(QUEUE));

  auto subscriber_callback =
      [this](const turtlesim_msgs::msg::Pose::ConstSharedPtr msg) -> void {
    this->x.store(msg->x);
    this->y.store(msg->y);
    this->theta.store(msg->theta);
    this->pose_received.store(true);
  };

  this->subscriber = this->create_subscription<turtlesim_msgs::msg::Pose>(
      "/moving_turtle/pose", rclcpp::SensorDataQoS(), subscriber_callback);

  this->action_server =
      rclcpp_action::create_server<software_training::action::Software>(
          this, "moving_turtle_action_server",
          std::bind(&moving_turtle_action_server::handle_goal, this, _1, _2),
          std::bind(&moving_turtle_action_server::handle_cancel, this, _1),
          std::bind(&moving_turtle_action_server::handle_accepted, this, _1));
}

rclcpp_action::GoalResponse moving_turtle_action_server::handle_goal(
    const rclcpp_action::GoalUUID &uuid,
    std::shared_ptr<const software_training::action::Software::Goal> goal) {
  (void)uuid;
  if (!std::isfinite(goal->x) || !std::isfinite(goal->y)) {
    RCLCPP_WARN(this->get_logger(), "Rejecting a non-finite waypoint");
    return rclcpp_action::GoalResponse::REJECT;
  }
  if (goal_active.exchange(true)) {
    RCLCPP_WARN(this->get_logger(), "A waypoint is already being executed");
    return rclcpp_action::GoalResponse::REJECT;
  }
  RCLCPP_INFO(this->get_logger(), "Waypoint received: x=%.2f y=%.2f",
              goal->x, goal->y);
  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse moving_turtle_action_server::handle_cancel(
    const std::shared_ptr<GoalHandleActionServer> goal_handle) {
  (void)goal_handle;
  RCLCPP_INFO(this->get_logger(), "Received request to cancel goal!");
  return rclcpp_action::CancelResponse::ACCEPT;
}

void moving_turtle_action_server::handle_accepted(
    const std::shared_ptr<GoalHandleActionServer> goal_handle) {
  std::thread([this](const std::shared_ptr<GoalHandleActionServer> handle) {
    this->execute(handle);
  }, goal_handle).detach();
}

void moving_turtle_action_server::execute(
    const std::shared_ptr<GoalHandleActionServer> goal_handle) {

  const auto goal = goal_handle->get_goal();
  const auto start_time = this->now();
  auto feedback = std::make_shared<software_training::action::Software::Feedback>();
  auto result = std::make_shared<software_training::action::Software::Result>();

  RCLCPP_INFO(this->get_logger(), "Executing Goal");
  rclcpp::WallRate cycle_rate(20.0);

  const double target_x = goal->x;
  const double target_y = goal->y;

  const auto set_duration = [this, &start_time, &result]() {
    result->duration = static_cast<builtin_interfaces::msg::Duration>(
        this->now() - start_time);
  };
  const auto stop_turtle = [this]() {
    this->publisher->publish(geometry_msgs::msg::Twist());
  };

  // Do not calculate control commands from the default-initialized pose.
  const auto pose_deadline = std::chrono::steady_clock::now() + 2s;
  while (rclcpp::ok() && !pose_received.load()) {
    if (goal_handle->is_canceling()) {
      stop_turtle();
      set_duration();
      goal_active.store(false);
      goal_handle->canceled(result);
      return;
    }
    if (std::chrono::steady_clock::now() >= pose_deadline) {
      RCLCPP_ERROR(this->get_logger(), "No moving_turtle pose received");
      set_duration();
      goal_active.store(false);
      goal_handle->abort(result);
      return;
    }
    cycle_rate.sleep();
  }

  while (rclcpp::ok()) {
    if (goal_handle->is_canceling()) {
      RCLCPP_INFO(this->get_logger(), "Goal Canceled");
      stop_turtle();
      set_duration();
      goal_active.store(false);
      goal_handle->canceled(result);
      return;
    }

    const double dx = target_x - x.load();
    const double dy = target_y - y.load();
    const double distance = std::hypot(dx, dy);

    auto cmd = std::make_unique<geometry_msgs::msg::Twist>();
    if (distance > 0.05) {
      const double desired_heading = std::atan2(dy, dx);
      const double raw_angle_error = desired_heading - theta.load();
      const double angle_error = std::atan2(std::sin(raw_angle_error),
                                            std::cos(raw_angle_error));

      // Rotate toward the line first. Once aligned, move forward while making
      // small heading corrections; turtlesim ignores linear.y.
      cmd->angular.z = clamp_value(4.0 * angle_error, -2.0, 2.0);
      if (std::abs(angle_error) < 0.1) {
        cmd->linear.x = std::min(2.0, std::max(0.2, distance));
      }
    }
    this->publisher->publish(std::move(cmd));

    feedback->distance = static_cast<float>(distance);
    goal_handle->publish_feedback(feedback);

    if (distance < 0.1) {
      break;
    }

    cycle_rate.sleep();
  }

  if (rclcpp::ok()) {
    stop_turtle();
    set_duration();
    goal_active.store(false);
    goal_handle->succeed(result);
    RCLCPP_INFO(this->get_logger(), "Finish Executing Goal");
  } else {
    goal_active.store(false);
  }
}

} // namespace composition

#include <rclcpp_components/register_node_macro.hpp>

RCLCPP_COMPONENTS_REGISTER_NODE(composition::moving_turtle_action_server)
