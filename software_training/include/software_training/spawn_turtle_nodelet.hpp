#ifndef SPAWN_TURTLE_NODELET_HPP_
#define SPAWN_TURTLE_NODELET_HPP_

#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include <software_training/visibility.h>
#include <turtlesim_msgs/srv/spawn.hpp>

namespace composition {

class spawn_turtle_nodelet : public rclcpp::Node {

public:
  SOFTWARE_TRAINING_PUBLIC
  explicit spawn_turtle_nodelet(const rclcpp::NodeOptions &options);

private:
  rclcpp::Client<turtlesim_msgs::srv::Spawn>::SharedPtr client;
  rclcpp::TimerBase::SharedPtr timer;

  SOFTWARE_TRAINING_LOCAL
  void spawn_turtle();

  static const unsigned int NUMBER_OF_TURTLES{2};

  typedef struct turtle_info {
    float x_pos;
    float y_pos;
    float rad;
  } turtle_info;

  std::vector<std::string> turtle_names{"stationary_turtle", "moving_turtle"};
  std::vector<turtle_info> turtle_bio{{5.0f, 5.0f, 0.0f},
                                      {25.0f, 10.0f, 0.0f}};

  // map of turtle name to turtle information
  std::map<std::string, turtle_info> turtle_description;
};

} // namespace composition

#endif // SPAWN_TURTLE_NODELET_HPP_
