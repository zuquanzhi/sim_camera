#pragma once

#include <opencv2/opencv.hpp>
#include <camera_info_manager/camera_info_manager.hpp>
#include <cstdint>
#include <image_transport/image_transport.hpp>
#include <std_msgs/msg/int32.hpp>
#include <rclcpp/logging.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp/utilities.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <cv_bridge/cv_bridge.h>
#include <thread>
#include <mutex>
#include <chrono>

namespace sim_camera {

enum CAMMOD {
  UNINITIALIZED = -1,
  UNKNOWN = 0,

  AIMBOT = 1,

  SMALL_RUNE = 2,
  BIG_RUNE = 3,

  FIX_HIT_OUTPOST = 4,
  DYNAMIC_HIT_OUTPOST = 5,

  HIT_FARAWAY = 6,

  // Video playback controls
  FAST_FORWARD = 10,     // 快进
  SLOW_SPEED = 11,       // 慢速（0.3倍速）
  REVERSE = 12,          // 倒退
  NORMAL_PLAY = 13,      // 正常播放
  PAUSE = 14,            // 暂停
};

class SimCameraNode : public rclcpp::Node {
public:
  explicit SimCameraNode(const rclcpp::NodeOptions &options);
  ~SimCameraNode() override;

private:
  void declareParameters();

  rcl_interfaces::msg::SetParametersResult
  parametersCallback(const std::vector<rclcpp::Parameter> &parameters);

  struct ModeParams {
    double fps;
    int width;
    int height;
    double brightness;
    double contrast;
  } AimbotParams_, RuneParams_, OutpostParams_, HitFarawayParams_;

  void checkMode(const std_msgs::msg::Int32::SharedPtr msg);
  void setParamsForAimbot();
  void setParamsForRune();
  void setParamsForOutpost();
  void setParamsForHitFaraway();
  
  // Video playback control functions
  void handlePlaybackControl(int control_mode);
  void setFastForward();
  void setSlowSpeed();
  void setReverse();
  void setNormalPlay();
  void setPause();
  
  int lastMode_ = CAMMOD::UNINITIALIZED;

  void captureLoop();
  bool loadVideo(const std::string& video_path);
  std::string resolvePackagePath(const std::string& path);
  cv::Mat preprocessFrame(const cv::Mat& frame);

  sensor_msgs::msg::Image image_msg_;
  image_transport::CameraPublisher camera_pub_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr readpack_sub_;

  std::string camera_name_;
  std::unique_ptr<camera_info_manager::CameraInfoManager> camera_info_manager_;
  sensor_msgs::msg::CameraInfo camera_info_msg_;

  std::thread capture_thread_;
  std::mutex video_mutex_;
  bool running_;

  // Video capture parameters
  cv::VideoCapture video_cap_;
  std::string video_path_;
  double current_fps_;
  int current_width_;
  int current_height_;
  double current_brightness_;
  double current_contrast_;
  bool loop_video_;
  
  // Video playback control parameters
  enum PlaybackState {
    NORMAL,
    FAST_FORWARD_STATE,
    SLOW_SPEED_STATE,
    REVERSE_STATE,
    PAUSED
  };
  PlaybackState playback_state_;
  double playback_speed_multiplier_;  // 播放速度倍数
  int frame_skip_;                    // 跳帧数量
  bool reverse_playback_;             // 是否倒放
  int current_frame_pos_;             // 当前帧位置
  int total_frames_;                  // 总帧数

  OnSetParametersCallbackHandle::SharedPtr params_callback_handle_;
};

} // namespace sim_camera
