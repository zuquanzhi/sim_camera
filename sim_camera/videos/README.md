# 仿真相机包视频文件夹

请将您的视频文件放在此目录中。

支持的视频格式：
- MP4
- AVI
- MOV
- MKV
- 其他OpenCV支持的格式

示例用法：
1. 将视频文件复制到此目录，例如 `sample.mp4`
2. 在配置文件中设置 `video_path` 参数为 `package://sim_camera/videos/sample.mp4`
3. 或者在启动时传递参数：`ros2 launch sim_camera sim_camera.launch.py video_path:=/path/to/your/video.mp4`

注意：如果使用相对路径，请确保文件存在且可访问。
