cmake_minimum_required(VERSION 3.10)
project(software_training)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(turtlesim REQUIRED)
find_package(turtlesim_msgs REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rcutils REQUIRED)
find_package(rcl REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)

include_directories(
  include
  ${rclcpp_INCLUDE_DIRS}
  ${rclcpp_components_INCLUDE_DIRS}
  ${turtlesim_INCLUDE_DIRS}
  ${turtlesim_msgs_INCLUDE_DIRS}
  ${std_msgs_INCLUDE_DIRS}
  ${std_srvs_INCLUDE_DIRS}
  ${rcutils_INCLUDE_DIRS}
  ${rcl_INCLUDE_DIRS}
  ${rclcpp_action_INCLUDE_DIRS}
  ${geometry_msgs_INCLUDE_DIRS}
  ${builtin_interfaces_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# custom services and messages and actions
find_package(rosidl_default_generators REQUIRED)
rosidl_generate_interfaces(${PROJECT_NAME}
	"msg/Software.msg"
	"srv/Software.srv"
	"action/Software.action"
	DEPENDENCIES std_msgs geometry_msgs builtin_interfaces
)
ament_export_dependencies(rosidl_default_runtime)

#create resource which references the libraries in the binary bin
set(node_plugins "")

#add plugins as SHARED library 

#add turtle_request_service_node as a plugin
add_library(turtle_request SHARED 
	src/turtle_neutralize.cpp)
target_compile_definitions(turtle_request
	PRIVATE "COMPOSITION_BUILDING_DLL")
target_link_libraries(turtle_request PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${std_srvs_LIBRARIES})
rclcpp_components_register_nodes(turtle_request "composition::turtle_service_request_node")
# this way we can execute the component with - ros2 component standalone software_training composition::turtle_service_request_node
set(node_plugins "${node_plugins}composition::turtle_service_request_node;$<TARGET_FILE:turtle_request>\n")


#add spawn_turtle_nodelets as plugin
add_library(turtle_spawn SHARED
	src/spawn_turtle_nodelet.cpp)
target_compile_definitions(turtle_spawn
	PRIVATE "COMPOSITION_BUILDING_DLL")
target_link_libraries(turtle_spawn PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${turtlesim_LIBRARIES} ${turtlesim_msgs_LIBRARIES} ${std_msgs_LIBRARIES})
rclcpp_components_register_nodes(turtle_spawn "composition::spawn_turtle_nodelet")
set(node_plugins "${node_plugins}composition::spawn_turtle_nodelet;$<TARGET_FILE:turtle_spawn>\n")


#add turtle publisher nodelet
add_library(turtle_pub SHARED
	src/turtle_publisher.cpp)
target_compile_definitions(turtle_pub
	PRIVATE "COMPOSITION_BUILDING_DLL"
	)
target_link_libraries(turtle_pub PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${turtlesim_LIBRARIES} ${turtlesim_msgs_LIBRARIES} ${std_msgs_LIBRARIES} ${PROJECT_NAME}__rosidl_generator_cpp ${PROJECT_NAME}__rosidl_typesupport_cpp)
rclcpp_components_register_nodes(turtle_pub "composition::turtle_publisher")
set(node_plugins "${node_plugins}composition::turtle_publisher;$<TARGET_FILE:turtle_pub>\n")

#add moving turtle service 
add_library(turtle_service SHARED
	src/reset_moving_turtle_service.cpp)
target_compile_definitions(turtle_service
	PRIVATE "COMPOSITION_BUILDING_DLL"
	)
target_link_libraries(turtle_service PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${turtlesim_LIBRARIES} ${turtlesim_msgs_LIBRARIES} ${std_msgs_LIBRARIES} ${PROJECT_NAME}__rosidl_generator_cpp ${PROJECT_NAME}__rosidl_typesupport_cpp)
rclcpp_components_register_nodes(turtle_service "composition::reset_moving_turtle_service")
set(node_plugins "${node_plugins}composition::reset_moving_turtle_service;$<TARGET_FILE:turtle_service>\n")

#add cmd_vel publisher for moving_turtle 
add_library(cmd_vel_publisher SHARED
	src/cmd_vel_moving_turt_publisher.cpp)
target_compile_definitions(cmd_vel_publisher 
	PRIVATE "COMPOSITION_BUILDING_DLL"
	)
target_link_libraries(cmd_vel_publisher PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${turtlesim_LIBRARIES} ${turtlesim_msgs_LIBRARIES} ${geometry_msgs_LIBRARIES} ${std_msgs_LIBRARIES})
rclcpp_components_register_nodes(cmd_vel_publisher "composition::cmd_vel_moving_turt_publisher")
set(node_plugins "${node_plugins}composition::cmd_vel_moving_turt_publisher;$<TARGET_FILE:cmd_vel_publisher>\n")

#add action server for moving turtle
#add_library(moving_turt_action SHARED
#	src/moving_turtle_action_server.cpp)
#target_compile_definitions(moving_turt_action 
#	PRIVATE "COMPOSITION_BUILDING_DLL"
#	)
#ament_target_dependencies(moving_turt_action
#	"rclcpp"
#	"rclcpp_components"
#	"turtlesim"
#	"rclcpp_action"
#	"geometry_msgs"
#	"std_msgs")
#rosidl_target_interfaces(moving_turt_action ${PROJECT_NAME} "rosidl_typesupport_cpp") # needed for custom action
#rclcpp_components_register_nodes(moving_turt_action "composition::moving_turtle_action_server")
#set(node_plugins "${node_plugins}composition::moving_turt_action_server;$<TARGET_FILE:moving_turt_action\n>")

add_library(turtle_action_server SHARED
	src/moving_turtle_action_server.cpp)
target_compile_definitions(turtle_action_server
	PRIVATE "SOFTWARE_TRAINING_DLL")
target_link_libraries(turtle_action_server PUBLIC ${rclcpp_LIBRARIES} ${rclcpp_components_LIBRARIES} ${turtlesim_LIBRARIES} ${turtlesim_msgs_LIBRARIES} ${rclcpp_action_LIBRARIES} ${std_msgs_LIBRARIES} ${geometry_msgs_LIBRARIES} ${PROJECT_NAME}__rosidl_generator_cpp ${PROJECT_NAME}__rosidl_typesupport_cpp)
# with this we can exexute the component as a node - ros2 run software_training moving_action_server
rclcpp_components_register_node(turtle_action_server PLUGIN "composition::moving_turtle_action_server" EXECUTABLE moving_action_server)



#tell where to put binaries
install(TARGETS
	turtle_request
	turtle_spawn
	turtle_pub
	turtle_service
	#moving_turt_action
	turtle_action_server
	cmd_vel_publisher
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib 
	RUNTIME DESTINATION bin)

#install launch file
install(DIRECTORY
	launch
	DESTINATION share/${PROJECT_NAME}
	)

ament_package()
