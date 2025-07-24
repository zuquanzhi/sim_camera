#include "keyboard_controller.hpp"
#include <iostream>

namespace sim_camera {

// 播放控制命令定义
enum PLAYBACK_CONTROLS {
  FAST_FORWARD = 10,
  SLOW_SPEED = 11,
  REVERSE = 12,
  NORMAL_PLAY = 13,
  PAUSE = 14,
  
  // 相机模式
  AIMBOT = 1,
  SMALL_RUNE = 2,
  BIG_RUNE = 3,
  FIX_HIT_OUTPOST = 4,
  DYNAMIC_HIT_OUTPOST = 5,
  HIT_FARAWAY = 6,
};

KeyboardController::KeyboardController(const rclcpp::NodeOptions &options)
    : Node("keyboard_controller", options), running_(true), terminal_configured_(false) {
  
  RCLCPP_INFO(this->get_logger(), "Starting Keyboard Controller!");
  
  // 创建发布者
  mode_publisher_ = this->create_publisher<std_msgs::msg::Int32>("/mode_command", 10);
  
  // 设置终端
  setupTerminal();
  
  // 打印帮助信息
  printHelp();
  
  // 启动键盘监听线程
  keyboard_thread_ = std::thread(&KeyboardController::keyboardLoop, this);
  
  RCLCPP_INFO(this->get_logger(), "Keyboard Controller ready! Press keys to control video playback.");
}

KeyboardController::~KeyboardController() {
  running_ = false;
  
  if (keyboard_thread_.joinable()) {
    keyboard_thread_.join();
  }
  
  restoreTerminal();
  RCLCPP_INFO(this->get_logger(), "Keyboard Controller destroyed!");
}

void KeyboardController::setupTerminal() {
  // 获取当前终端设置
  if (tcgetattr(STDIN_FILENO, &original_termios_) == 0) {
    struct termios new_termios = original_termios_;
    
    // 设置为非规范模式，关闭回显
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0;   // 非阻塞读取
    new_termios.c_cc[VTIME] = 1;  // 100ms 超时
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_termios) == 0) {
      terminal_configured_ = true;
      
      // 设置非阻塞输入
      int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
      fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
  }
}

void KeyboardController::restoreTerminal() {
  if (terminal_configured_) {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios_);
    
    // 恢复阻塞模式
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
  }
}

char KeyboardController::getKeyPress() {
  char key = 0;
  if (read(STDIN_FILENO, &key, 1) > 0) {
    return key;
  }
  return 0;
}

void KeyboardController::keyboardLoop() {
  while (rclcpp::ok() && running_) {
    char key = getKeyPress();
    if (key != 0) {
      processKey(key);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
}

void KeyboardController::processKey(char key) {
  auto msg = std_msgs::msg::Int32();
  bool valid_key = true;
  
  switch (key) {
    // 播放控制键
    case 'f':  // Fast forward - 快进
    case 'F':
      msg.data = FAST_FORWARD;
      RCLCPP_INFO(this->get_logger(), "Fast Forward (2x speed)");
      break;
      
    case 's':  // Slow speed - 慢速
    case 'S':
      msg.data = SLOW_SPEED;
      RCLCPP_INFO(this->get_logger(), "Slow Speed (0.3x speed)");
      break;
      
    case 'r':  // Reverse - 倒退
    case 'R':
      msg.data = REVERSE;
      RCLCPP_INFO(this->get_logger(), "Reverse Playback");
      break;
      
    case 'n':  // Normal play - 正常播放
    case 'N':
    case ' ':  // 空格键也可以恢复正常播放
      msg.data = NORMAL_PLAY;
      RCLCPP_INFO(this->get_logger(), "Normal Playback");
      break;
      
    case 'p':  // Pause - 暂停
    case 'P':
      msg.data = PAUSE;
      RCLCPP_INFO(this->get_logger(), "Pause");
      break;
      
    // 相机模式快捷键
    case '1':
      msg.data = AIMBOT;
      RCLCPP_INFO(this->get_logger(), "Switch to AIMBOT mode");
      break;
      
    case '2':
      msg.data = SMALL_RUNE;
      RCLCPP_INFO(this->get_logger(), "Switch to SMALL_RUNE mode");
      break;
      
    case '3':
      msg.data = BIG_RUNE;
      RCLCPP_INFO(this->get_logger(), "Switch to BIG_RUNE mode");
      break;
      
    case '4':
      msg.data = FIX_HIT_OUTPOST;
      RCLCPP_INFO(this->get_logger(), "Switch to FIX_HIT_OUTPOST mode");
      break;
      
    case '5':
      msg.data = DYNAMIC_HIT_OUTPOST;
      RCLCPP_INFO(this->get_logger(), "Switch to DYNAMIC_HIT_OUTPOST mode");
      break;
      
    case '6':
      msg.data = HIT_FARAWAY;
      RCLCPP_INFO(this->get_logger(), "Switch to HIT_FARAWAY mode");
      break;
      
    // 帮助和退出
    case 'h':
    case 'H':
    case '?':
      printHelp();
      valid_key = false;
      break;
      
    case 'q':
    case 'Q':
    case 27:  // ESC key
      RCLCPP_INFO(this->get_logger(), "Exiting keyboard controller...");
      running_ = false;
      rclcpp::shutdown();
      valid_key = false;
      break;
      
    default:
      valid_key = false;
      break;
  }
  
  if (valid_key) {
    mode_publisher_->publish(msg);
  }
}

void KeyboardController::printHelp() {
  std::cout << "\n========== 假相机键盘控制器 ==========" << std::endl;
  std::cout << "视频播放控制:" << std::endl;
  std::cout << "  F/f  - 快进 (2倍速)" << std::endl;
  std::cout << "  S/s  - 慢速 (0.3倍速)" << std::endl;
  std::cout << "  R/r  - 倒退播放" << std::endl;
  std::cout << "  N/n  - 正常播放" << std::endl;
  std::cout << "  P/p  - 暂停" << std::endl;
  std::cout << "  空格 - 正常播放" << std::endl;
  std::cout << std::endl;
  std::cout << "相机模式切换:" << std::endl;
  std::cout << "  1    - AIMBOT 模式" << std::endl;
  std::cout << "  2    - SMALL_RUNE 模式" << std::endl;
  std::cout << "  3    - BIG_RUNE 模式" << std::endl;
  std::cout << "  4    - FIX_HIT_OUTPOST 模式" << std::endl;
  std::cout << "  5    - DYNAMIC_HIT_OUTPOST 模式" << std::endl;
  std::cout << "  6    - HIT_FARAWAY 模式" << std::endl;
  std::cout << std::endl;
  std::cout << "其他:" << std::endl;
  std::cout << "  H/h/?- 显示此帮助" << std::endl;
  std::cout << "  Q/q  - 退出程序" << std::endl;
  std::cout << "  ESC  - 退出程序" << std::endl;
  std::cout << "=====================================" << std::endl;
  std::cout << "提示: 直接按键即可，无需回车确认" << std::endl;
  std::cout << std::endl;
}

} // namespace sim_camera

#include "rclcpp_components/register_node_macro.hpp"
RCLCPP_COMPONENTS_REGISTER_NODE(sim_camera::KeyboardController)
