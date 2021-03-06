// Copyright (c) 2019-2020 Ricardo Tonet
// Use of this source code is governed by the MIT license, see LICENSE

/**
 * \file move_to_start.cpp
 * \brief ROS node to move the robot arm to a start pose.
 */

#include <ros/ros.h>
#include <smalldrop_toolpath/trajectory_planner.h>
#include <smalldrop_toolpath/trajectory.h>
#include <smalldrop_toolpath/line.h>

// ROS messages
#include <geometry_msgs/Pose.h>
#include <trajectory_msgs/JointTrajectory.h>

using namespace smalldrop::smalldrop_toolpath;

/**
 * Global variables
 *********************************************************************************************/

pose_t initial_pose;
bool has_pose = false;
int freq = 100; // 100 Hz
double ttime = 10; // Trajectory duration in seconds

/**
 * Function prototypes
 *********************************************************************************************/

void currentPoseCallback(geometry_msgs::Pose::ConstPtr msg);

/**
 * Main
 *********************************************************************************************/

int main(int argc, char **argv)
{
  ros::init(argc, argv, "move_to_start");
  ros::NodeHandle nh;

  ros::Publisher pose_pub = nh.advertise<geometry_msgs::Pose>("/smalldrop/robot_arm/desired_pose", 10);
  ros::Publisher hand_pub = nh.advertise<trajectory_msgs::JointTrajectory>("/panda_hand_controller/command", 10);
  ros::Subscriber sub = nh.subscribe("/smalldrop/robot_arm/current_pose", 10, currentPoseCallback);

  // Process command-line arguments
  int opt;
  const char* const short_opts = ":hT:F:";
  while ((opt = getopt(argc, argv, short_opts)) != -1)
  {
    switch (opt)
    {
      case 'T':
        ttime = std::stod(optarg);
        break;
      case 'F':
        freq = std::stoi(optarg);
        break;
      case 'h':
      default:
        std::cout << "Help:" << std::endl; 
        std::cout << "move_to_start -h -T <trajectory_time> -F <frequency>" << std::endl; 
        return 0;
    }
  } 

  // Process current pose callback
  ros::Rate r(10);
  while (ros::ok() && !has_pose)
  {
    ros::spinOnce();
    r.sleep();
  }

  // Close pand hand fingers
  trajectory_msgs::JointTrajectory fingers_pose;
  fingers_pose.header.stamp = ros::Time::now();
  fingers_pose.joint_names.resize(2);
  fingers_pose.joint_names[0] = "panda_finger_joint1";
  fingers_pose.joint_names[1] = "panda_finger_joint2";
  std::vector<trajectory_msgs::JointTrajectoryPoint> points_n(1);
  points_n[0].positions.resize(1);
  points_n[0].positions.push_back(0.0);
  points_n[0].time_from_start = ros::Duration(5);
  fingers_pose.points = points_n;
  hand_pub.publish(fingers_pose);

  // Define start pose (to where the robot should move)
  pose_t final_pose;
  final_pose.position.x = 0.55;
  final_pose.position.y = 0.0;
  final_pose.position.z = 0.53;
  final_pose.orientation.x = 1.0;
  final_pose.orientation.y = 0.0;
  final_pose.orientation.z = 0.0;
  final_pose.orientation.w = -0.0;

  // Plan linear trajectory between inital_pose and final_pose
  Line line(initial_pose, final_pose);
  TrajectoryPlanner pl(ttime, freq, PLAN_MODE::POLY3);  
  Trajectory t(pl.plan(line));
  poses_t traj = t.poses();
  
  ros::Rate r2(freq);
  for (size_t i = 0; i < traj.size(); i++)
  {
    // Send trajectory to robot
    pose_pub.publish(traj[i]);
    ros::spinOnce();
    r2.sleep();
  }
  
  return 0;
}

/**
 * General functions & callbacks
 *********************************************************************************************/

void currentPoseCallback(geometry_msgs::Pose::ConstPtr msg)
{
  initial_pose.position.x = msg->position.x;
  initial_pose.position.y = msg->position.y;
  initial_pose.position.z = msg->position.z;
  initial_pose.orientation.x = msg->orientation.x;
  initial_pose.orientation.y = msg->orientation.y;
  initial_pose.orientation.z = msg->orientation.z;
  initial_pose.orientation.w = msg->orientation.w;
  has_pose = true;
}