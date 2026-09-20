import launch
from launch.actions import TimerAction
from launch_ros.actions import ComposableNodeContainer, Node
from launch_ros.descriptions import ComposableNode


def generate_launch_description():
    """Launch the turtlesim simulator and all ROS training components."""
    turtlesim_node = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='simulator',
        output='screen',
        parameters=[{
            'background_r': 255,
            'background_g': 255,
            'background_b': 255,
        }],
    )

    container = ComposableNodeContainer(
        name='software_training_container',
        namespace='',
        package='rclcpp_components',
        executable='component_container',
        arguments=['--executor-type', 'multi-threaded'],
        composable_node_descriptions=[
            ComposableNode(
                package='software_training',
                plugin='composition::turtle_service_request_node',
                name='turtle_request',
            ),
            ComposableNode(
                package='software_training',
                plugin='composition::spawn_turtle_nodelet',
                name='spawn_turtle_nodelet',
            ),
            ComposableNode(
                package='software_training',
                plugin='composition::turtle_publisher',
                name='turtle_publisher',
            ),
            ComposableNode(
                package='software_training',
                plugin='composition::reset_moving_turtle_service',
                name='reset_moving_turtle_service',
            ),
            ComposableNode(
                package='software_training',
                plugin='composition::cmd_vel_moving_turt_publisher',
                name='cmd_vel_moving_turt_publisher',
            ),
            ComposableNode(
                package='software_training',
                plugin='composition::moving_turtle_action_server',
                name='moving_turtle_action_server',
            ),
        ],
        output='screen',
    )

    # Start turtlesim first, then give it time to advertise its services before
    # loading components that depend on them.
    delayed_container = TimerAction(period=1.0, actions=[container])
    return launch.LaunchDescription([turtlesim_node, delayed_container])
