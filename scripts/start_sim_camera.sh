#!/bin/bash

# Sim Camera 启动脚本
# 用于简化启动流程

echo "========================================="
echo "    Sim Camera ROS2 仿真相机启动脚本"
echo "========================================="
echo ""

# 检查ROS2环境
if [ -z "$ROS_DISTRO" ]; then
    echo "❌ 错误：未检测到ROS2环境"
    echo "请先运行：source /opt/ros/humble/setup.bash"
    echo "或者：source your_workspace/install/setup.bash"
    exit 1
fi

echo "✅ ROS2环境：$ROS_DISTRO"
echo ""

# 选择启动模式
echo "请选择启动模式："
echo "1) 仅启动假相机"
echo "2) 启动假相机 + 键盘控制器（推荐）"
echo "3) 查看帮助信息"
echo ""
read -p "请输入选择 (1-3): " choice

case $choice in
    1)
        echo ""
        echo "🎬 启动假相机..."
        ros2 launch sim_camera sim_camera.launch.py
        ;;
    2)
        echo ""
        echo "🎬 启动假相机..."
        echo "⚠️  请在另一个终端中运行键盘控制器："
        echo "   ros2 run sim_camera sim_camera_keyboard_main"
        echo ""
        echo "🎮 键盘快捷键："
        echo "   F - 快进 (2倍速)"
        echo "   S - 慢速 (0.3倍速)" 
        echo "   R - 倒退播放"
        echo "   N/空格 - 正常播放"
        echo "   P - 暂停"
        echo "   H - 显示帮助"
        echo "   Q - 退出"
        echo ""
        read -p "按回车键继续启动假相机..."
        ros2 launch sim_camera sim_camera.launch.py
        ;;
    3)
        echo ""
        echo "📖 使用帮助："
        echo ""
        echo "1. 基本启动："
        echo "   ros2 launch sim_camera sim_camera.launch.py"
        echo ""
        echo "2. 指定视频文件："
        echo "   ros2 launch sim_camera sim_camera.launch.py video_path:=/path/to/video.mp4"
        echo ""
        echo "3. 键盘控制器（需要在另一个终端运行）："
        echo "   ros2 run sim_camera sim_camera_keyboard_main"
        echo ""
        echo "4. 模式切换（通过话题）："
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 1}\""
        echo ""
        echo "5. 播放控制（通过话题）："
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 10}\"  # 快进"
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 11}\"  # 慢速"
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 12}\"  # 倒退"
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 13}\"  # 正常"
        echo "   ros2 topic pub /mode_command std_msgs/msg/Int32 \"{data: 14}\"  # 暂停"
        echo ""
        ;;
    *)
        echo "❌ 无效选择，退出"
        exit 1
        ;;
esac
