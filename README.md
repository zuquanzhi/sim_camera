# Sim Camera - ROS2 仿真相机包

基于 `rm_camdrv` 开发的仿真相机包，读取本地视频文件伪装成相机，用于算法调试和仿真测试。

## ✨ 功能特性

- 📹 **视频播放**：支持多种格式，循环播放
- 🎮 **键盘控制**：快捷键控制播放（快进/倒退/暂停）
- 🎛️ **实时调整**：动态调整帧率、分辨率、亮度、对比度
- 🔄 **模式切换**：兼容原包的6种模式（AIMBOT、RUNE、OUTPOST等）
- � **ROS2标准**：标准的图像和相机信息发布

##  快速开始

### 1. 编译安装
```bash
cd your_workspace
colcon build --packages-select sim_camera
source install/setup.bash
```

### 2. 启动使用

#### 🚀 快速启动（推荐）
```bash
# 使用启动脚本（交互式选择）
./scripts/start_sim_camera.sh
```

#### 🎮 键盘控制模式

**方式1：分别启动（推荐）**
```bash
# 终端1：启动假相机
ros2 launch sim_camera sim_camera.launch.py

# 终端2：启动键盘控制器（需要在支持键盘输入的终端中运行）
ros2 run sim_camera sim_camera_keyboard_main
```

**方式2：同时启动**
```bash
# 在支持键盘输入的终端中运行（注意：某些IDE终端可能不支持）
ros2 launch sim_camera sim_camera_with_keyboard.launch.py
```

> ⚠️ **注意**：键盘控制器需要在真正的终端（如系统Terminal、iTerm2等）中运行，某些IDE内置终端可能不支持键盘输入捕获。

#### 📹 基本模式
```bash
# 使用默认视频
ros2 launch sim_camera sim_camera.launch.py

# 指定视频文件
ros2 launch sim_camera sim_camera.launch.py video_path:=/path/to/video.mp4
```

## 🎮 键盘快捷键

### 视频播放控制

| 按键 | 功能 |
|------|------|
| **F** | 快进 (2倍速) |
| **S** | 慢速 (0.3倍速) |
| **R** | 倒退播放 |
| **Space** / **N** | 正常播放 |
| **P** | 暂停 |

### 相机模式切换

| 按键 | 模式 |
|------|------|
| **1** | AIMBOT |
| **2** | SMALL_RUNE |
| **3** | BIG_RUNE |
| **4** | FIX_HIT_OUTPOST |
| **5** | DYNAMIC_HIT_OUTPOST |
| **6** | HIT_FARAWAY |

### 其他

| 按键 | 功能 |
|------|------|
| **H** / **?** | 显示帮助 |
| **Q** / **ESC** | 退出 |

> 💡 **提示**：直接按键即可，无需回车确认

## ⚙️ 实时参数控制

### 命令行调整
```bash
# 调整播放参数
ros2 param set /sim_camera fps 60.0
ros2 param set /sim_camera width 1280
ros2 param set /sim_camera height 720
ros2 param set /sim_camera brightness 0.1
ros2 param set /sim_camera contrast 1.2

# 模式切换
ros2 topic pub --once /mode_command std_msgs/msg/Int32 "{data: 1}"  # AIMBOT模式
```

### 播放控制命令
```bash
# 通过话题控制播放
ros2 topic pub /mode_command std_msgs/msg/Int32 "data: 10"  # 快进
ros2 topic pub /mode_command std_msgs/msg/Int32 "data: 11"  # 慢速
ros2 topic pub /mode_command std_msgs/msg/Int32 "data: 12"  # 倒退
ros2 topic pub /mode_command std_msgs/msg/Int32 "data: 13"  # 正常
ros2 topic pub /mode_command std_msgs/msg/Int32 "data: 14"  # 暂停
```

### 程序控制示例
```cpp
// 创建发布者
auto publisher = node->create_publisher<std_msgs::msg::Int32>("/mode_command", 10);

// 发送快进命令
auto msg = std_msgs::msg::Int32();
msg.data = 10;  // FAST_FORWARD
publisher->publish(msg);

// 发送慢速命令
msg.data = 11;  // SLOW_SPEED
publisher->publish(msg);

// 发送倒退命令
msg.data = 12;  // REVERSE
publisher->publish(msg);
```

## � 项目结构

```
sim_camera/
├── config/                    # 配置文件
│   ├── camera_info.yaml      # 相机标定信息
│   └── camera_params.yaml    # 参数配置
├── include/
│   ├── sim_camera.hpp        # 主要头文件
│   └── keyboard_controller.hpp  # 键盘控制器头文件
├── src/
│   ├── sim_camera_node.cpp   # 主节点
│   ├── camera_modes.cpp      # 模式切换
│   ├── keyboard_controller.cpp      # 键盘控制器
│   └── keyboard_controller_main.cpp # 键盘控制器启动
├── launch/
│   ├── sim_camera.launch.py         # 基本启动文件
│   ├── sim_camera_only.launch.py    # 仅启动假相机
│   └── sim_camera_with_keyboard.launch.py  # 含键盘控制启动文件
├── scripts/
│   └── start_sim_camera.sh          # 交互式启动脚本
└── videos/
    └── Movie_002.avi         # 示例视频
```

## �📡 ROS2 接口

### 发布话题
- `/image` - 图像数据 (sensor_msgs/Image)
- `/camera_info` - 相机标定信息 (sensor_msgs/CameraInfo)

### 订阅话题
- `/mode_command` - 模式切换和播放控制 (std_msgs/Int32)

### 主要参数

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| video_path | string | - | 视频文件路径（支持package://格式） |
| fps | double | 30.0 | 播放帧率 |
| width/height | int | 640/480 | 输出分辨率 |
| brightness | double | 0.0 | 亮度调整 (-1.0 到 1.0) |
| contrast | double | 1.0 | 对比度调整 (0.0 到 3.0) |
| loop_video | bool | true | 是否循环播放 |

## 🎯 模式系统

### 支持的模式
1. **AIMBOT (1)** - 自瞄模式：60fps, 对比度1.2
2. **SMALL_RUNE (2)** - 小符模式：30fps, 亮度+0.1
3. **BIG_RUNE (3)** - 大符模式：30fps, 亮度+0.1  
4. **FIX_HIT_OUTPOST (4)** - 固定前哨站：45fps, 800x600
5. **DYNAMIC_HIT_OUTPOST (5)** - 动态前哨站：45fps, 800x600
6. **HIT_FARAWAY (6)** - 远距离：60fps, 1280x720, 对比度1.3

### 播放控制模式
- **FAST_FORWARD (10)** - 快进：2倍速
- **SLOW_SPEED (11)** - 慢速：0.3倍速
- **REVERSE (12)** - 倒退播放
- **NORMAL_PLAY (13)** - 正常播放
- **PAUSE (14)** - 暂停播放

## 🔧 技术实现

### 播放控制原理
1. **快进功能**: 通过增加播放速度倍数(2.0x)和跳帧(1帧)实现
2. **慢速功能**: 通过降低播放速度倍数(0.3x)，不跳帧确保流畅性
3. **倒退功能**: 通过反向设置视频帧位置实现倒放
4. **暂停功能**: 在捕获循环中暂停帧读取
5. **正常播放**: 恢复默认播放参数(1.0x速度)
6. **键盘控制**: 使用非阻塞终端输入，实时响应按键

### 状态监控
播放状态变化会在日志中显示：
- `Set to fast forward mode (2x speed)`
- `Set to slow speed mode (0.3x speed)` 
- `Set to reverse playback mode`
- `Set to normal playback mode`
- `Video paused`

## 📹 视频文件管理

### 支持格式
MP4, AVI, MOV, MKV 等OpenCV支持的格式

### 路径配置
```yaml
# 推荐：使用包内路径
video_path: "package://sim_camera/videos/your_video.mp4"

# 或使用绝对路径
video_path: "/path/to/your/video.mp4"
```

## 🔧 配置示例

### camera_params.yaml
```yaml
/sim_camera:
  ros__parameters:
    video_path: "package://sim_camera/videos/Movie_002.avi"
    fps: 30.0
    width: 640
    height: 480
    brightness: 0.0
    contrast: 1.0
    loop_video: true
    
    # 各模式特定参数
    Aimbot: {fps: 60.0, width: 640, height: 480, brightness: 0.0, contrast: 1.2}
    Rune: {fps: 30.0, width: 640, height: 480, brightness: 0.1, contrast: 1.0}
    # ... 更多配置
```

## � 兼容性说明

### 与 rm_camdrv 对比

| 项目 | rm_camdrv | sim_camera |
|------|-----------|------------|
| 图像话题 | ✅ /image | ✅ /image |
| 模式切换 | /Readpack | /mode_command |
| 模式值 | ✅ 1-6 | ✅ 1-6 + 播放控制10-14 |
| 数据源 | 硬件相机 | 视频文件 |

### 迁移步骤
1. 将话题 `/Readpack` 改为 `/mode_command`
2. 准备测试视频文件
3. 调整相关参数配置

## 💡 使用建议

- **开发调试**：使用键盘控制模式，实时调整播放速度
- **自动化测试**：使用命令行模式，脚本化控制
- **性能优化**：根据算法需求配置合适的分辨率和帧率
- **多模式测试**：为不同模式准备对应的测试视频
- **终端选择**：键盘控制器建议在系统终端（Terminal、iTerm2）中运行，避免使用IDE内置终端

## ⚠️ 常见问题

### 1. 键盘控制器无响应
- **原因**：在不支持键盘输入的终端中运行
- **解决**：使用系统Terminal、iTerm2或其他真正的终端应用

### 2. xterm错误
- **原因**：系统中没有安装xterm
- **解决**：使用分别启动的方式，避免使用组合启动文件

### 3. 视频文件找不到
- **原因**：视频路径配置错误
- **解决**：检查video_path参数，确保文件存在

### 4. 播放控制不响应
- **原因**：话题名称错误或节点未启动
- **解决**：确认使用正确话题名`/mode_command`，检查节点状态

## 🔧 技术要求

### 系统依赖
键盘控制器使用了以下系统调用：
- `termios.h` - 终端控制
- `fcntl.h` - 文件控制  
- `unistd.h` - POSIX系统调用

这些在Linux和macOS系统中都可用。

## 🛠️ 依赖要求

```bash
# ROS2包依赖
sudo apt install ros-humble-cv-bridge ros-humble-image-transport 
sudo apt install ros-humble-camera-info-manager

# 系统依赖
sudo apt install libopencv-dev
```

---

**适用场景**：算法调试、仿真测试、离线数据处理、算法验证
