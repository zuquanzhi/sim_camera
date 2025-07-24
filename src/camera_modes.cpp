#include "sim_camera.hpp"

namespace sim_camera {

void SimCameraNode::checkMode(const std_msgs::msg::Int32::SharedPtr msg) {
  // Check if it's a playback control command
  if (msg->data >= CAMMOD::FAST_FORWARD && msg->data <= CAMMOD::PAUSE) {
    handlePlaybackControl(msg->data);
    return;
  }

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

void SimCameraNode::handlePlaybackControl(int control_mode) {
  std::lock_guard<std::mutex> lock(video_mutex_);
  
  switch (control_mode) {
  case CAMMOD::FAST_FORWARD:
    setFastForward();
    break;
  case CAMMOD::SLOW_SPEED:
    setSlowSpeed();
    break;
  case CAMMOD::REVERSE:
    setReverse();
    break;
  case CAMMOD::NORMAL_PLAY:
    setNormalPlay();
    break;
  case CAMMOD::PAUSE:
    setPause();
    break;
  default:
    RCLCPP_WARN(this->get_logger(), "Unknown playback control: %d", control_mode);
  }
}

void SimCameraNode::setFastForward() {
  playback_state_ = FAST_FORWARD_STATE;
  playback_speed_multiplier_ = 2.0;  // 2倍速快进
  frame_skip_ = 1;  // 每次跳过1帧
  reverse_playback_ = false;
  RCLCPP_INFO(this->get_logger(), "Set to fast forward mode (2x speed)");
}

void SimCameraNode::setSlowSpeed() {
  playback_state_ = SLOW_SPEED_STATE;
  playback_speed_multiplier_ = 0.3;  // 0.3倍速慢放
  frame_skip_ = 0;  // 不跳帧
  reverse_playback_ = false;
  RCLCPP_INFO(this->get_logger(), "Set to slow speed mode (0.3x speed)");
}

void SimCameraNode::setReverse() {
  playback_state_ = REVERSE_STATE;
  playback_speed_multiplier_ = 1.0;
  frame_skip_ = 0;
  reverse_playback_ = true;
  RCLCPP_INFO(this->get_logger(), "Set to reverse playback mode");
}

void SimCameraNode::setNormalPlay() {
  playback_state_ = NORMAL;
  playback_speed_multiplier_ = 1.0;
  frame_skip_ = 0;
  reverse_playback_ = false;
  RCLCPP_INFO(this->get_logger(), "Set to normal playback mode");
}

void SimCameraNode::setPause() {
  playback_state_ = PAUSED;
  RCLCPP_INFO(this->get_logger(), "Video paused");
}

}
