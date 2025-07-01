#include "sim_camera.hpp"

namespace sim_camera {

void SimCameraNode::checkMode(const std_msgs::msg::Int32::SharedPtr msg) {
  if (msg->data == lastMode_) {
    return; 
  }

  switch (msg->data) {
  case CAMMOD::AIMBOT:
    setParamsForAimbot();
    break;
  case CAMMOD::SMALL_RUNE:
  case CAMMOD::BIG_RUNE:
    setParamsForRune();
    break;
  case CAMMOD::FIX_HIT_OUTPOST:
  case CAMMOD::DYNAMIC_HIT_OUTPOST:
    setParamsForOutpost();
    break;
  case CAMMOD::HIT_FARAWAY:
    setParamsForHitFaraway();
    break;
  default:
    RCLCPP_WARN(this->get_logger(), "UNKNOWN MODE: %d", msg->data);
  }
  RCLCPP_INFO(this->get_logger(), "SWITCH TO MODE %d", msg->data);
  lastMode_ = msg->data;
}

void SimCameraNode::setParamsForAimbot() {
  this->set_parameter(rclcpp::Parameter("fps", AimbotParams_.fps));
  this->set_parameter(rclcpp::Parameter("width", AimbotParams_.width));
  this->set_parameter(rclcpp::Parameter("height", AimbotParams_.height));
  this->set_parameter(rclcpp::Parameter("brightness", AimbotParams_.brightness));
  this->set_parameter(rclcpp::Parameter("contrast", AimbotParams_.contrast));
}

void SimCameraNode::setParamsForRune() {
  this->set_parameter(rclcpp::Parameter("fps", RuneParams_.fps));
  this->set_parameter(rclcpp::Parameter("width", RuneParams_.width));
  this->set_parameter(rclcpp::Parameter("height", RuneParams_.height));
  this->set_parameter(rclcpp::Parameter("brightness", RuneParams_.brightness));
  this->set_parameter(rclcpp::Parameter("contrast", RuneParams_.contrast));
}

void SimCameraNode::setParamsForOutpost() {
  this->set_parameter(rclcpp::Parameter("fps", OutpostParams_.fps));
  this->set_parameter(rclcpp::Parameter("width", OutpostParams_.width));
  this->set_parameter(rclcpp::Parameter("height", OutpostParams_.height));
  this->set_parameter(rclcpp::Parameter("brightness", OutpostParams_.brightness));
  this->set_parameter(rclcpp::Parameter("contrast", OutpostParams_.contrast));
}

void SimCameraNode::setParamsForHitFaraway() {
  this->set_parameter(rclcpp::Parameter("fps", HitFarawayParams_.fps));
  this->set_parameter(rclcpp::Parameter("width", HitFarawayParams_.width));
  this->set_parameter(rclcpp::Parameter("height", HitFarawayParams_.height));
  this->set_parameter(rclcpp::Parameter("brightness", HitFarawayParams_.brightness));
  this->set_parameter(rclcpp::Parameter("contrast", HitFarawayParams_.contrast));
}

}
