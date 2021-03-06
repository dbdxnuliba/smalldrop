cmake_minimum_required(VERSION 2.8.3)
project(smalldrop_robot_arm)

add_compile_options(-std=c++11 -Wall -g)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  controller_manager
  joint_state_controller
  robot_state_publisher
  dynamic_reconfigure
  geometry_msgs
  tf
  kdl_parser
  gazebo_ros
  franka_hw
  smalldrop_toolpath
)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Eigen3 REQUIRED)
find_package(gazebo REQUIRED)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

generate_dynamic_reconfigure_options(
  cfg/CartesianImpedanceSimController.cfg
  cfg/CartesianImpedanceRealController.cfg
  cfg/JointPositionPIDSimController.cfg
)

###################################
## catkin specific configuration ##
###################################
catkin_package(
 INCLUDE_DIRS include
 LIBRARIES ${PROJECT_NAME}
 CATKIN_DEPENDS
  roscpp
  controller_manager
  joint_state_controller
  robot_state_publisher
  dynamic_reconfigure
  geometry_msgs
  tf
  kdl_parser
  gazebo_ros
  franka_hw
  smalldrop_toolpath
 DEPENDS 
  Boost
  Eigen3
)

###########
## Build ##
###########

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${GAZEBO_INCLUDE_DIRS}
)
link_directories(${GAZEBO_LIBRARY_DIRS})
list(APPEND CMAKE_CXX_FLAGS "${GAZEBO_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")

add_library(sdrop_robot_real_cartesian_impedance 
  src/controllers/cartesian_impedance_real_controller.cpp
  src/controllers/robot_arm_controller.cpp
)
add_library(sdrop_robot_real_gravity_compensation 
  src/controllers/gravity_compensation_real_controller.cpp
  src/controllers/robot_arm_controller.cpp
)
add_library(sdrop_robot_sim_cartesian_impedance 
  src/controllers/cartesian_impedance_sim_controller.cpp
  src/controllers/robot_arm_controller.cpp
)
add_library(sdrop_robot_sim_joint_position_pid
  src/controllers/joint_position_pid_sim_controller.cpp
  src/controllers/robot_arm_joint_controller.cpp
)
add_library(sdrop_create_sim_world SHARED src/gazebo_plugins/create_sim_world.cc)

add_executable(move_to_start src/nodes/move_to_start.cpp)
add_executable(move_to_joint_config src/nodes/move_to_joint_config.cpp)
add_executable(execute_trajectory src/nodes/execute_trajectory.cpp)

add_dependencies(sdrop_robot_real_cartesian_impedance ${PROJECT_NAME}_gencfg ${PROJECT_NAME}_generate_messages_cpp)
add_dependencies(sdrop_robot_real_gravity_compensation ${PROJECT_NAME}_generate_messages_cpp)
add_dependencies(sdrop_robot_sim_cartesian_impedance ${PROJECT_NAME}_gencfg ${PROJECT_NAME}_generate_messages_cpp)
add_dependencies(sdrop_robot_sim_joint_position_pid ${PROJECT_NAME}_gencfg ${PROJECT_NAME}_generate_messages_cpp)

target_link_libraries(sdrop_robot_real_cartesian_impedance ${catkin_LIBRARIES})
target_link_libraries(sdrop_robot_real_gravity_compensation ${catkin_LIBRARIES})
target_link_libraries(sdrop_robot_sim_cartesian_impedance ${catkin_LIBRARIES})
target_link_libraries(sdrop_robot_sim_joint_position_pid ${catkin_LIBRARIES})
target_link_libraries(sdrop_create_sim_world ${GAZEBO_LIBRARIES})

target_link_libraries(move_to_start path_planner trajectory_planner ${catkin_LIBRARIES})
target_link_libraries(move_to_joint_config joint_trajectory_planner ${catkin_LIBRARIES})
target_link_libraries(execute_trajectory path_planner trajectory_planner ${catkin_LIBRARIES})

#############
## Install ##
#############

#############
## Testing ##
#############