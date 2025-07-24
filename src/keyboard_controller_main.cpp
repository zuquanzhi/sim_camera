#include "keyboard_controller.hpp"

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  
  auto node = std::make_shared<sim_camera::KeyboardController>();
  
  try {
    rclcpp::spin(node);
  } catch (const std::exception& e) {
    RCLCPP_ERROR(node->get_logger(), "Exception in keyboard controller: %s", e.what());
  }
  
  rclcpp::shutdown();
  return 0;
}
