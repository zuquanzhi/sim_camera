#include "sim_camera.hpp"

namespace sim_camera {

SimCameraNode::SimCameraNode(const rclcpp::NodeOptions &options)
    : Node("sim_camera", options), running_(false) {
  RCLCPP_INFO(this->get_logger(), "Starting SimCameraNode!");

  bool use_sensor_data_qos =
      this->declare_parameter("use_sensor_data_qos", true);
  auto qos = use_sensor_data_qos ? rmw_qos_profile_sensor_data
                                 : rmw_qos_profile_default;
  camera_pub_ =
      image_transport::create_camera_publisher(this, "image", qos);

  // Load camera info
  camera_name_ = this->declare_parameter("camera_name", "sim_camera");
  camera_info_manager_ =
      std::make_unique<camera_info_manager::CameraInfoManager>(this,
                                                               camera_name_);
  auto camera_info_url = this->declare_parameter(
      "camera_info_url", "package://sim_camera/config/camera_info.yaml");
  if (camera_info_manager_->validateURL(camera_info_url)) {
    camera_info_manager_->loadCameraInfo(camera_info_url);
    camera_info_msg_ = camera_info_manager_->getCameraInfo();
  } else {
    RCLCPP_WARN(this->get_logger(), "Invalid camera info URL: %s",
                camera_info_url.c_str());
  }

  declareParameters();

  if (!this->has_parameter("video_path")) {
    this->declare_parameter("video_path", std::string(""));
  }
  video_path_ = this->get_parameter("video_path").as_string();
  RCLCPP_INFO(this->get_logger(), "Video path from parameter: '%s'", video_path_.c_str());
  if (video_path_.empty()) {
    RCLCPP_ERROR(this->get_logger(), "No video path specified!");
    return;
  }

  std::string resolved_path = resolvePackagePath(video_path_);
  RCLCPP_INFO(this->get_logger(), "Resolved video path: '%s'", resolved_path.c_str());

  if (!loadVideo(resolved_path)) {
    RCLCPP_ERROR(this->get_logger(), "Failed to load video: %s", video_path_.c_str());
    return;
  }

  params_callback_handle_ = this->add_on_set_parameters_callback(
      std::bind(&SimCameraNode::parametersCallback, this, std::placeholders::_1));

  readpack_sub_ = this->create_subscription<std_msgs::msg::Int32>(
      "/mode_command", 1,
      std::bind(&SimCameraNode::checkMode, this, std::placeholders::_1));

  running_ = true;
  capture_thread_ = std::thread(&SimCameraNode::captureLoop, this);

  RCLCPP_INFO(this->get_logger(), "SimCameraNode initialized successfully!");
}

SimCameraNode::~SimCameraNode() {
  running_ = false;
  if (capture_thread_.joinable()) {
    capture_thread_.join();
  }
  if (video_cap_.isOpened()) {
    video_cap_.release();
  }
  RCLCPP_INFO(this->get_logger(), "SimCameraNode destroyed!");
}

void SimCameraNode::declareParameters() {
  rcl_interfaces::msg::ParameterDescriptor param_desc;

  param_desc.description = "Frames per second";
  current_fps_ = this->declare_parameter("fps", 30.0, param_desc);

  param_desc.description = "Video width";
  current_width_ = this->declare_parameter("width", 640, param_desc);

  param_desc.description = "Video height";
  current_height_ = this->declare_parameter("height", 480, param_desc);

  param_desc.description = "Brightness adjustment";
  current_brightness_ = this->declare_parameter("brightness", 0.0, param_desc);

  param_desc.description = "Contrast adjustment";
  current_contrast_ = this->declare_parameter("contrast", 1.0, param_desc);

  param_desc.description = "Loop video when it ends";
  loop_video_ = this->declare_parameter("loop_video", true, param_desc);

  AimbotParams_.fps = this->declare_parameter("Aimbot.fps", 60.0);
  AimbotParams_.width = this->declare_parameter("Aimbot.width", 640);
  AimbotParams_.height = this->declare_parameter("Aimbot.height", 480);
  AimbotParams_.brightness = this->declare_parameter("Aimbot.brightness", 0.0);
  AimbotParams_.contrast = this->declare_parameter("Aimbot.contrast", 1.2);

  RuneParams_.fps = this->declare_parameter("Rune.fps", 30.0);
  RuneParams_.width = this->declare_parameter("Rune.width", 640);
  RuneParams_.height = this->declare_parameter("Rune.height", 480);
  RuneParams_.brightness = this->declare_parameter("Rune.brightness", 0.1);
  RuneParams_.contrast = this->declare_parameter("Rune.contrast", 1.0);

  OutpostParams_.fps = this->declare_parameter("Outpost.fps", 45.0);
  OutpostParams_.width = this->declare_parameter("Outpost.width", 800);
  OutpostParams_.height = this->declare_parameter("Outpost.height", 600);
  OutpostParams_.brightness = this->declare_parameter("Outpost.brightness", 0.0);
  OutpostParams_.contrast = this->declare_parameter("Outpost.contrast", 1.1);

  HitFarawayParams_.fps = this->declare_parameter("HitFaraway.fps", 60.0);
  HitFarawayParams_.width = this->declare_parameter("HitFaraway.width", 1280);
  HitFarawayParams_.height = this->declare_parameter("HitFaraway.height", 720);
  HitFarawayParams_.brightness = this->declare_parameter("HitFaraway.brightness", 0.0);
  HitFarawayParams_.contrast = this->declare_parameter("HitFaraway.contrast", 1.3);
}

rcl_interfaces::msg::SetParametersResult SimCameraNode::parametersCallback(
    const std::vector<rclcpp::Parameter> &parameters) {
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  
  std::lock_guard<std::mutex> lock(video_mutex_);
  
  for (const auto &param : parameters) {
    if (param.get_name() == "fps") {
      current_fps_ = param.as_double();
      RCLCPP_INFO(this->get_logger(), "FPS: %f", current_fps_);
    } else if (param.get_name() == "width") {
      current_width_ = param.as_int();
      RCLCPP_INFO(this->get_logger(), "Width: %d", current_width_);
    } else if (param.get_name() == "height") {
      current_height_ = param.as_int();
      RCLCPP_INFO(this->get_logger(), "Height: %d", current_height_);
    } else if (param.get_name() == "brightness") {
      current_brightness_ = param.as_double();
      RCLCPP_INFO(this->get_logger(), "Brightness: %f", current_brightness_);
    } else if (param.get_name() == "contrast") {
      current_contrast_ = param.as_double();
      RCLCPP_INFO(this->get_logger(), "Contrast: %f", current_contrast_);
    } else if (param.get_name() == "loop_video") {
      loop_video_ = param.as_bool();
      RCLCPP_INFO(this->get_logger(), "Loop video: %s", loop_video_ ? "true" : "false");
    } else {
      RCLCPP_WARN(this->get_logger(), "Unknown parameter: %s", param.get_name().c_str());
    }
  }
  return result;
}

bool SimCameraNode::loadVideo(const std::string& video_path) {
  std::lock_guard<std::mutex> lock(video_mutex_);
  
  if (video_cap_.isOpened()) {
    video_cap_.release();
  }
  
  video_cap_.open(video_path);
  if (!video_cap_.isOpened()) {
    RCLCPP_ERROR(this->get_logger(), "Cannot open video file: %s", video_path.c_str());
    return false;
  }
  
  RCLCPP_INFO(this->get_logger(), "Successfully loaded video: %s", video_path.c_str());
  return true;
}

cv::Mat SimCameraNode::preprocessFrame(const cv::Mat& frame) {
  cv::Mat processed_frame = frame.clone();
  
  if (processed_frame.cols != current_width_ || processed_frame.rows != current_height_) {
    cv::resize(processed_frame, processed_frame, cv::Size(current_width_, current_height_));
  }
  
  if (current_brightness_ != 0.0 || current_contrast_ != 1.0) {
    processed_frame.convertTo(processed_frame, -1, current_contrast_, current_brightness_ * 255);
  }
  
  return processed_frame;
}

void SimCameraNode::captureLoop() {
  cv::Mat frame;
  
  RCLCPP_INFO(this->get_logger(), "Starting video capture loop!");
  
  while (rclcpp::ok() && running_) {
    auto loop_start = std::chrono::steady_clock::now();
    
    {
      std::lock_guard<std::mutex> lock(video_mutex_);
      
      if (!video_cap_.isOpened()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
      }
      
      video_cap_ >> frame;
      
      if (frame.empty()) {
        if (loop_video_) {

          video_cap_.set(cv::CAP_PROP_POS_FRAMES, 0);
          RCLCPP_INFO(this->get_logger(), "Looping video");
          continue;
        } else {
          RCLCPP_WARN(this->get_logger(), "End of video reached");
          break;
        }
      }
    }
    
    cv::Mat processed_frame = preprocessFrame(frame);
    
    auto cv_image = cv_bridge::CvImage();
    cv_image.header.stamp = this->now();
    cv_image.header.frame_id = "camera_optical_frame";
    cv_image.encoding = "bgr8";
    cv_image.image = processed_frame;
    
    image_msg_ = *cv_image.toImageMsg();
    
    camera_info_msg_.header = image_msg_.header;
    camera_info_msg_.width = current_width_;
    camera_info_msg_.height = current_height_;
    
    camera_pub_.publish(image_msg_, camera_info_msg_);
    
    if (current_fps_ > 0) {
      auto frame_duration = std::chrono::duration<double>(1.0 / current_fps_);
      auto loop_end = std::chrono::steady_clock::now();
      auto elapsed = loop_end - loop_start;
      auto sleep_time = frame_duration - elapsed;
      
      if (sleep_time > std::chrono::duration<double>(0)) {
        std::this_thread::sleep_for(sleep_time);
      }
    }
  }
  
  RCLCPP_INFO(this->get_logger(), "Capture loop ended");
}

std::string SimCameraNode::resolvePackagePath(const std::string& path) {
  const std::string package_prefix = "package://";
  if (path.substr(0, package_prefix.length()) == package_prefix) {
    std::string remaining = path.substr(package_prefix.length());
    size_t slash_pos = remaining.find('/');
    
    if (slash_pos != std::string::npos) {
      std::string package_name = remaining.substr(0, slash_pos);
      std::string relative_path = remaining.substr(slash_pos + 1);
      
      try {
        std::string package_share_dir = ament_index_cpp::get_package_share_directory(package_name);
        return package_share_dir + "/" + relative_path;
      } catch (const std::exception& e) {
        RCLCPP_ERROR(this->get_logger(), "Failed to resolve package path '%s': %s", path.c_str(), e.what());
        return path;
      }
    }
  }
  
  return path;
}

}

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(sim_camera::SimCameraNode)
