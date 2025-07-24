import os
from typing import Dict, Any

from ament_index_python.packages import get_package_share_directory # type: ignore
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction # type: ignore
from launch.substitutions import LaunchConfiguration # type: ignore
from launch_ros.actions import Node # type: ignore


def launch_setup(context):
    """Launch setup function to conditionally add parameters"""
    params_file = os.path.join(
        get_package_share_directory('sim_camera'), 'config', 'camera_params.yaml')

    camera_info_url = 'package://sim_camera/config/camera_info.yaml'
    
    video_path_value = LaunchConfiguration('video_path').perform(context)
    
    base_params = {
        'camera_info_url': LaunchConfiguration('camera_info_url'),
        'use_sensor_data_qos': LaunchConfiguration('use_sensor_data_qos'),
    }
    
    if video_path_value and video_path_value.strip():
        base_params['video_path'] = LaunchConfiguration('video_path')
    
    node = Node(
        package='sim_camera',
        executable='sim_camera_node',
        output='screen',
        emulate_tty=True,
        parameters=[
            LaunchConfiguration('params_file'),
            base_params
        ],
    )
    
    return [node]


def generate_launch_description():
    params_file = os.path.join(
        get_package_share_directory('sim_camera'), 'config', 'camera_params.yaml')

    camera_info_url = 'package://sim_camera/config/camera_info.yaml'

    return LaunchDescription([
        DeclareLaunchArgument(
            name='params_file',
            default_value=params_file,
            description='Full path to the camera parameters file'
        ),
        DeclareLaunchArgument(
            name='camera_info_url',
            default_value=camera_info_url,
            description='Camera info calibration URL'
        ),
        DeclareLaunchArgument(
            name='use_sensor_data_qos',
            default_value='true',
            description='Use sensor data QoS for image topics'
        ),
        DeclareLaunchArgument(
            name='video_path',
            default_value='',
            description='Video file path (optional, overrides config file if specified)'
        ),

        OpaqueFunction(function=launch_setup)
    ])
