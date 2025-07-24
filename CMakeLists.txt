cmake_minimum_required(VERSION 3.8)
project(sim_camera)

## Use C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## By adding -Wall and -Werror, the compiler does not ignore warnings anymore,
## enforcing cleaner code.
add_definitions(-Wall -Werror)

## Export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ament_cmake_auto REQUIRED)
find_package(OpenCV REQUIRED)
ament_auto_find_build_dependencies()

ament_auto_add_library(${PROJECT_NAME} SHARED
  src/sim_camera_node.cpp
  src/camera_modes.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

rclcpp_components_register_node(${PROJECT_NAME}
  PLUGIN sim_camera::SimCameraNode
  EXECUTABLE ${PROJECT_NAME}_node
)

# Add keyboard controller library
ament_auto_add_library(${PROJECT_NAME}_keyboard SHARED
  src/keyboard_controller.cpp
)

target_include_directories(${PROJECT_NAME}_keyboard PUBLIC include/)

rclcpp_components_register_node(${PROJECT_NAME}_keyboard
  PLUGIN sim_camera::KeyboardController
  EXECUTABLE ${PROJECT_NAME}_keyboard_node
)

# Add standalone keyboard controller executable
ament_auto_add_executable(${PROJECT_NAME}_keyboard_main
  src/keyboard_controller_main.cpp
  src/keyboard_controller.cpp
)

target_include_directories(${PROJECT_NAME}_keyboard_main PUBLIC include/)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  list(APPEND AMENT_LINT_AUTO_EXCLUDE
    ament_cmake_copyright
    ament_cmake_cpplint
    ament_cmake_uncrustify
  )
  ament_lint_auto_find_test_dependencies()
endif()


ament_auto_package(
  INSTALL_TO_SHARE
  config
  launch
  videos
)
