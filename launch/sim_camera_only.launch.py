#!/usr/bin/env python3

from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    # 声明启动参数
    declare_video_path = DeclareLaunchArgument(
        'video_path',
        default_value=os.path.join(
            get_package_share_directory('sim_camera'),
            'videos', 'Movie_002.avi'
        ),
        description='Path to the video file'
    )
    
    declare_camera_info_url = DeclareLaunchArgument(
        'camera_info_url',
        default_value=os.path.join(
            get_package_share_directory('sim_camera'),
            'config', 'camera_info.yaml'
        ),
        description='Path to camera info file'
    )
    
    # 只启动假相机节点
    sim_camera_node = Node(
        package='sim_camera',
        executable='sim_camera_node',
        name='sim_camera',
        parameters=[{
            'video_path': LaunchConfiguration('video_path'),
            'camera_info_url': LaunchConfiguration('camera_info_url'),
            'fps': 30.0,
            'width': 640,
            'height': 480,
            'brightness': 0.0,
            'contrast': 1.0,
            'loop_video': True,
            'use_sensor_data_qos': True,
            'camera_name': 'sim_camera',
            # 不同模式的参数
            'Aimbot.fps': 60.0,
            'Aimbot.width': 640,
            'Aimbot.height': 480,
            'Aimbot.brightness': 0.0,
            'Aimbot.contrast': 1.2,
            'Rune.fps': 30.0,
            'Rune.width': 640,
            'Rune.height': 480,
            'Rune.brightness': 0.1,
            'Rune.contrast': 1.0,
            'Outpost.fps': 45.0,
            'Outpost.width': 800,
            'Outpost.height': 600,
            'Outpost.brightness': 0.0,
            'Outpost.contrast': 1.1,
            'HitFaraway.fps': 60.0,
            'HitFaraway.width': 1280,
            'HitFaraway.height': 720,
            'HitFaraway.brightness': 0.0,
            'HitFaraway.contrast': 1.3,
        }],
        output='screen'
    )
    
    return LaunchDescription([
        declare_video_path,
        declare_camera_info_url,
        sim_camera_node,
    ])
