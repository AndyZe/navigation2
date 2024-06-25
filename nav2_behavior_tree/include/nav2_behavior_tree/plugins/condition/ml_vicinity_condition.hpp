// Copyright (c) 2024 Andy Zelenak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NAV2_BEHAVIOR_TREE__PLUGINS__CONDITION__ML_VICINITY_CONDITION_HPP_
#define NAV2_BEHAVIOR_TREE__PLUGINS__CONDITION__ML_VICINITY_CONDITION_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include <string>

namespace nav2_behavior_tree
{

/**
 * @brief A BT::ConditionNode, returns SUCCESS when a large language model says the vicinity is clear
 *        The input data has type sensor_msgs::msg::Image.
 */
class MLVicinityCondition : public BT::ConditionNode
{
public:
  /**
   * @brief A constructor for nav2_behavior_tree::MLVicinityCondition
   * @param condition_name Name for the XML tag for this node
   * @param conf BT node configuration
   */
  MLVicinityCondition(
    const std::string & condition_name,
    const BT::NodeConfiguration & conf);

  MLVicinityCondition() = delete;

  /**
   * @brief The main override required by a BT action
   * @return BT::NodeStatus Status of tick execution
   */
  BT::NodeStatus tick() override;

  /**
   * @brief Creates list of BT ports
   * @return BT::PortsList Containing node-specific ports
   */
  static BT::PortsList providedPorts()
  {
    return {
      BT::InputPort<std::string>("image_topic", "Image topic which is subscribed to"),
      BT::InputPort<std::chrono::milliseconds>("server_timeout")
    };
  }

private:
  /**
   * @brief Capture the latest image to send to the ML model
   *
   * @param msg
   */
  void imageCallback(const sensor_msgs::msg::Image& msg)
  {
    const std::lock_guard<std::mutex> lock(image_mutex_);
    latest_image_ = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::TYPE_32FC1);
  }

  /**
  * @brief Send the prompt (or otherwise run) the AI model. This includes the latest image as input.
  *
  * @return true if successful
  */
  [[nodiscard]] bool promptAIModel();

  rclcpp::Node::SharedPtr node_;
  // The timeout value while waiting for a response from the
  // is path valid service
  std::chrono::milliseconds server_timeout_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;
  mutable std::mutex image_mutex_;
  std::optional<cv_bridge::CvImageConstPtr> latest_image_;
};

}  // namespace nav2_behavior_tree

#endif  // NAV2_BEHAVIOR_TREE__PLUGINS__CONDITION__ML_VICINITY_CONDITION_
