// Copyright (c) 2019-2020 Ricardo Tonet
// Use of this source code is governed by the MIT license, see LICENSE

/**
 * \file system_state.cpp
 * \brief Defines a class that models the smalldrop system state.
 */

#include <smalldrop_state/system_state.h>

namespace smalldrop
{
namespace smalldrop_state
{

/*****************************************************************************************
 * Public methods & constructors/destructors
 *****************************************************************************************/

SystemState::SystemState()
{
  // Topics
  system_state_topic_ = "/smalldrop/bioprint/state";
  remote_ctrl_state_topic_ = "/spacenav/joy";
  robot_arm_current_pose_topic_ = "/smalldrop/robot_arm/current_pose";
  robot_arm_desired_pose_topic_ = "/smalldrop/robot_arm/desired_pose";
  robot_arm_current_joint_pos_topic_ = "/smalldrop/robot_arm/current_joint_positions";
  robot_arm_desired_joint_pos_topic_ = "/smalldrop/robot_arm/desired_joint_positions";
  rviz_segmentation_points_topic_ = "/smalldrop/teleoperation/segmentation_points";

  // Service topics
  controller_manager_switch_controller_srv_topic_ = "/controller_manager/switch_controller";

  remote_ctrl_state_.axes.reserve(6);
  remote_ctrl_state_.buttons.reserve(2);
  remote_ctrl_state_.axes[0] = 0;
  remote_ctrl_state_.axes[1] = 0;
  remote_ctrl_state_.axes[2] = 0;
  remote_ctrl_state_.axes[3] = 0;
  remote_ctrl_state_.axes[4] = 0;
  remote_ctrl_state_.axes[5] = 0;
  remote_ctrl_state_.buttons[0] = 0;
  remote_ctrl_state_.buttons[1] = 0;

  subscribeTopics();
}

/**
 * \copybrief SystemState::getSystemState() const
 */
std::string SystemState::getSystemState() const
{
  return system_state_;
}

/**
 * \copybrief SystemState::getRobotArmPose() const
 */
geometry_msgs::Pose SystemState::getRobotArmPose() const
{
  return robot_arm_pose_;
}

/**
 * \copybrief SystemState::getRobotArmJointPositions() const
 */
smalldrop_msgs::JointPositions SystemState::getRobotArmJointPositions() const
{
  return robot_arm_joint_pos_;
}

/**
 * \copybrief SystemState::getRemoteCtrlState() const
 */
sensor_msgs::Joy SystemState::getRemoteCtrlState() const
{
  return remote_ctrl_state_;
}

/**
 * \copybrief SystemState::getRobotDesiredPosePublisher() const
 */
ros::Publisher SystemState::getRobotDesiredPosePublisher() const
{
  return robot_arm_desired_pose_pub_;
}

/**
 * \copybrief SystemState::getRobotDesiredJointPositionsPublisher() const
 */
ros::Publisher SystemState::getRobotDesiredJointPositionsPublisher() const
{
  return robot_arm_desired_joint_pos_pub_;
}

/**
 * \copybrief SystemState::getTrajectoryMarkersPublisher() const
 */
ros::Publisher SystemState::getTrajectoryMarkersPublisher() const
{
  return rviz_trajectory_markers_pub_;
}

/**
 * \copybrief SystemState::getSegmentationPointsPublisher() const
 */
ros::Publisher SystemState::getSegmentationPointsPublisher() const
{
  return rviz_segmentation_points_pub_;
}
/**
 * \copybrief SystemState::getControllerManagerSwitchControllerService() const
 */
ros::ServiceClient SystemState::getControllerManagerSwitchControllerService() const
{
  return controller_manager_switch_controller_srv_;
}

/*****************************************************************************************
 * Private methods
 *****************************************************************************************/

/**
 * \copybrief SystemState::subscribeTopics()
 */
void SystemState::subscribeTopics()
{
  system_state_sub_ = nh_.subscribe<std_msgs::String>(system_state_topic_, 10, &SystemState::systemStateCallback, this);
  robot_arm_state_sub_ = nh_.subscribe<geometry_msgs::Pose>(robot_arm_current_pose_topic_, 10, &SystemState::robotArmStateCallback, this);
  robot_arm_joint_pos_sub_ = nh_.subscribe<smalldrop_msgs::JointPositions>(robot_arm_current_joint_pos_topic_, 10, &SystemState::robotArmJointPositionsCallback, this);
  remote_ctrl_state_sub_ = nh_.subscribe<sensor_msgs::Joy>(remote_ctrl_state_topic_, 10, &SystemState::remoteCtrlStateCallback, this);

  robot_arm_desired_pose_pub_ = nh_.advertise<geometry_msgs::Pose>(robot_arm_desired_pose_topic_, 10);
  robot_arm_desired_joint_pos_pub_ = nh_.advertise<smalldrop_msgs::JointPositions>(robot_arm_desired_joint_pos_topic_, 10);
  rviz_segmentation_points_pub_ = nh_.advertise<visualization_msgs::Marker>(rviz_segmentation_points_topic_, 10);

  // Services
  controller_manager_switch_controller_srv_ = nh_.serviceClient<controller_manager_msgs::SwitchController>(controller_manager_switch_controller_srv_topic_);
}

/**
 * \copybrief SystemState::systemStateCallback()
 */
void SystemState::systemStateCallback(const std_msgs::String::ConstPtr &msg)
{
  system_state_ = msg->data;
}

/**
 * \copybrief SystemState::robotArmStateCallback()
 */
void SystemState::robotArmStateCallback(const geometry_msgs::Pose::ConstPtr &msg)
{
  robot_arm_pose_.position = msg->position;
  robot_arm_pose_.orientation = msg->orientation;
}

/**
 * \copybrief SystemState::robotArmJointPositionsCallback()
 */
void SystemState::robotArmJointPositionsCallback(const smalldrop_msgs::JointPositions::ConstPtr &msg)
{
  robot_arm_joint_pos_.names = msg->names;
  robot_arm_joint_pos_.positions = msg->positions;
}

/**
 * \copybrief SystemState::remoteCtrlStateCallback()
 */
void SystemState::remoteCtrlStateCallback(const sensor_msgs::Joy::ConstPtr &msg)
{
  remote_ctrl_state_.header = msg->header;
  remote_ctrl_state_.axes = msg->axes;
  remote_ctrl_state_.buttons = msg->buttons;
}

}  // namespace smalldrop_state

}  // namespace smalldrop