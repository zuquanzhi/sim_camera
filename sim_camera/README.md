# Sim Camera - ROS2 仿真相机包

这是一个基于原始 `rm_camdrv` 包开发的仿真相机包，可以读取本地视频文件并伪装成相机发布节点，用于算法调试和仿真测试。

## 🎯 功能特性

- 📹 **视频文件播放**：支持多种视频格式（MP4, AVI, MOV, MKV等）
- 🎛️ **实时参数控制**：可动态调整帧率、分辨率、亮度、对比度
- 🔄 **模式切换**：支持与原包相同的模式切换机制（AIMBOT, RUNE, OUTPOST, HIT_FARAWAY）
- 🔁 **循环播放**：支持视频循环播放
- 📊 **ROS2兼容**：完全兼容ROS2，发布标准的图像和相机信息消息
- 🔧 **易于部署**：支持 `package://` 路径格式，无需硬编码绝对路径

## 📁 项目结构

```
sim_camera/
├── CMakeLists.txt              # CMake构建文件
├── package.xml                 # ROS2包描述文件
├── README.md                   # 项目说明文档（本文件）
├── config/                     # 配置文件
│   ├── camera_params.yaml     # 相机参数配置
│   └── camera_info.yaml       # 相机标定信息
├── include/
│   └── sim_camera.hpp         # 主要头文件
├── src/
│   ├── sim_camera_node.cpp    # 主节点实现
│   └── camera_modes.cpp       # 模式切换功能
├── launch/
│   ├── sim_camera.launch.py   # 标准启动文件
│   └── sim_camera_with_test_video.launch.py  # 测试启动文件
├── scripts/
│   ├── test_mode_switch.py    # 模式切换测试脚本
│   ├── create_test_video.py   # 测试视频生成脚本
│   └── sim_camera_run.sh      # 快速运行脚本
└── videos/
    ├── Movie_002.avi          # 示例视频文件
    └── README.md              # 视频文件说明
```

## 🚀 快速开始

### 1. 安装依赖

```bash
sudo apt update
sudo apt install ros-humble-cv-bridge ros-humble-image-transport ros-humble-camera-info-manager
sudo apt install libopencv-dev python3-opencv
```

### 2. 编译

```bash
cd /path/to/your/workspace
colcon build --packages-select sim_camera
source install/setup.bash  # 或 install/setup.zsh
```

### 3. 启动仿真相机

#### 使用默认配置（推荐）：
```bash
ros2 launch sim_camera sim_camera.launch.py
```

#### 指定视频文件：
```bash
# 使用绝对路径
ros2 launch sim_camera sim_camera.launch.py video_path:=/path/to/your/video.mp4

# 使用包内路径（推荐）
ros2 launch sim_camera sim_camera.launch.py video_path:=package://sim_camera/videos/your_video.mp4
```

## ⚙️ 配置文件

### camera_params.yaml

主要配置文件，包含所有可调参数：

```yaml
/sim_camera:
  ros__parameters:
    camera_name: sim_camera
    
    # 视频文件路径（必需）
    video_path: "package://sim_camera/videos/Movie_002.avi"
    
    # 基本参数
    fps: 30.0
    width: 640
    height: 480
    brightness: 0.0  # -1.0 到 1.0
    contrast: 1.0    # 0.0 到 3.0
    loop_video: true
    
    # 模式特定参数
    Aimbot:
      fps: 60.0
      width: 640
      height: 480
      brightness: 0.0
      contrast: 1.2
    
    Small_rune:
      fps: 30.0
      width: 640
      height: 480
      brightness: 0.1
      contrast: 1.1
    
    Big_rune:
      fps: 30.0
      width: 640
      height: 480
      brightness: 0.1
      contrast: 1.1
    
    # ... 其他模式配置
```

## 🎮 运行时控制

### 实时参数调整

```bash
# 调整帧率
ros2 param set /sim_camera fps 60.0

# 调整分辨率
ros2 param set /sim_camera width 1280
ros2 param set /sim_camera height 720

# 调整图像属性
ros2 param set /sim_camera brightness 0.1
ros2 param set /sim_camera contrast 1.2

# 开启/关闭循环播放
ros2 param set /sim_camera loop_video true
```

### 模式切换

与原包保持一致的模式切换机制：

- **AIMBOT (1)**：高帧率自瞄模式
- **SMALL_RUNE/BIG_RUNE (2/3)**：符文识别模式  
- **FIX_HIT_OUTPOST/DYNAMIC_HIT_OUTPOST (4/5)**：前哨站模式
- **HIT_FARAWAY (6)**：远距离击打模式

```bash
# 切换到自瞄模式
ros2 topic pub --once /mode_command std_msgs/msg/Int32 "{data: 1}"

# 切换到符文模式
ros2 topic pub --once /mode_command std_msgs/msg/Int32 "{data: 2}"
```

## 📡 ROS2 接口

### 发布的话题

- `/image` - 图像数据 (sensor_msgs/Image)
- `/camera_info` - 相机标定信息 (sensor_msgs/CameraInfo)

### 订阅的话题

- `/mode_command` - 模式切换消息 (std_msgs/msg/Int32)

### 参数说明

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| video_path | string | "" | 视频文件路径（支持 package:// 格式） |
| fps | double | 30.0 | 发布帧率 |
| width | int | 640 | 输出宽度 |
| height | int | 480 | 输出高度 |
| brightness | double | 0.0 | 亮度调整 (-1.0 到 1.0) |
| contrast | double | 1.0 | 对比度调整 (0.0 到 3.0) |
| loop_video | bool | true | 是否循环播放 |
| camera_name | string | "sim_camera" | 相机名称 |
| use_sensor_data_qos | bool | true | 使用传感器数据QoS |

## 📹 视频文件管理

### 支持的格式

- MP4
- AVI  
- MOV
- MKV
- 其他OpenCV支持的格式

### 使用方法

1. **使用包内视频**（推荐）：
   - 将视频文件放在 `videos/` 目录中
   - 在配置文件中使用：`package://sim_camera/videos/your_video.mp4`

2. **使用绝对路径**：
   - 直接指定完整路径：`/path/to/your/video.mp4`

3. **启动时指定**：
   ```bash
   ros2 launch sim_camera sim_camera.launch.py video_path:=/path/to/video.mp4
   ```

## 🛠️ 工具脚本

### 测试视频生成

```bash
# 生成彩色测试视频
python3 scripts/create_test_video.py

# 使用测试视频启动
ros2 launch sim_camera sim_camera_with_test_video.launch.py
```

### 模式切换测试

```bash
# 自动化模式切换测试
python3 scripts/test_mode_switch.py
```

## 🔧 故障排除

### 1. "No video path specified!" 错误

**原因**：video_path 参数为空或未正确设置

**解决方案**：
- 检查 `config/camera_params.yaml` 中的 `video_path` 设置
- 确保使用正确的启动命令加载配置文件
- 使用 launch 文件而不是直接运行节点

### 2. 无法加载视频文件

**原因**：文件路径错误或文件不存在

**解决方案**：
- 检查文件路径是否正确（支持 `package://` 格式）
- 确认视频文件存在且格式被OpenCV支持
- 检查文件权限

### 3. 图像发布异常

**原因**：OpenCV或ROS2包缺失

**解决方案**：
- 重新安装OpenCV：`sudo apt install libopencv-dev`
- 安装ROS2图像包：`sudo apt install ros-humble-cv-bridge ros-humble-image-transport`

### 4. 参数设置无效

**调试命令**：
```bash
# 查看可用参数
ros2 param list

# 检查参数值
ros2 param get /sim_camera video_path

# 查看节点状态
ros2 node info /sim_camera
```

## 📊 与原包对比

| 特性 | rm_camdrv | sim_camera | 兼容性 |
|------|-----------|------------|--------|
| 图像话题 | /image | /image | ✅ 完全兼容 |
| 相机信息 | /camera_info | /camera_info | ✅ 完全兼容 |
| 模式切换话题 | /Readpack | /mode_command | ⚠️ 话题名不同 |
| 模式切换值 | 1-6 | 1-6 | ✅ 完全兼容 |
| 数据源 | 海康威视相机 | 本地视频文件 | ❌ 数据源不同 |
| 参数类型 | 曝光、增益 | 帧率、分辨率、图像调整 | ❌ 参数不同 |
| 标定系统 | camera_info_manager | camera_info_manager | ✅ 完全兼容 |

## 🔄 升级和兼容性

### 从 rm_camdrv 迁移

1. **话题订阅更改**：将 `/Readpack` 改为 `/mode_command`
2. **参数调整**：用图像处理参数替代相机硬件参数
3. **视频文件准备**：准备测试用视频文件

### 版本兼容性

- **ROS2版本**：支持 Humble 及以上版本
- **OpenCV版本**：支持 4.x 版本
- **Python版本**：支持 3.8 及以上版本

## 💡 使用建议

1. **测试环境**：先使用包内示例视频测试功能
2. **参数调优**：根据具体算法需求调整图像参数
3. **模式配置**：为不同模式配置合适的参数组合
4. **性能监控**：使用 `ros2 topic hz /image` 监控发布频率

## 🤝 贡献和支持

基于 `rm_camdrv` 包改写，保持了相同的接口和模式切换机制。

如有问题或建议，请检查：
1. 日志输出：查看详细错误信息
2. 参数设置：确认所有必需参数正确配置
3. 环境依赖：确认所有依赖包正确安装

---

**适用场景**：算法调试、仿真测试、离线数据处理、算法验证
