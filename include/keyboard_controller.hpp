#pragma once

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <atomic>

namespace sim_camera {

class KeyboardController : public rclcpp::Node {
public:
  explicit KeyboardController(const rclcpp::NodeOptions &options = rclcpp::NodeOptions());
  ~KeyboardController() override;

private:
  void keyboardLoop();
  void setupTerminal();
  void restoreTerminal();
  char getKeyPress();
  void processKey(char key);
  void printHelp();

  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr mode_publisher_;
  std::thread keyboard_thread_;
  std::atomic<bool> running_;
  
  struct termios original_termios_;
  bool terminal_configured_;
};

} // namespace sim_camera
