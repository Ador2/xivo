// Message types for interfacing the estimator with data acquisition module.
// Author: Xiaohan Fei (feixh@cs.ucla.edu)
#pragma once

#include <iostream>
#include <list>

#include "core.h"

namespace feh {

namespace msg {

struct Message {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  Message(const timestamp_t &ts) : ts_{ts} {}
  virtual ~Message() {}

  timestamp_t ts_;
};

struct Image : public Message {
  Image(const timestamp_t &ts, const std::string &image_path)
      : Message{ts}, image_path_{image_path} {}

  std::string image_path_;
};

struct IMU : public Message {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  IMU(const timestamp_t &ts, const Vec3 &gyro, const Vec3 &accel)
      : Message{ts}, gyro_{gyro}, accel_{accel} {}

  // friend std::ostream& operator<<(std::ostream& os, const IMU &msg);

  Vec3 gyro_, accel_;
};

struct Track : public Message {
  Track(const timestamp_t &ts, std::list<FeaturePtr> &&features)
      : Message{ts}, features_{features} {}

  std::list<FeaturePtr> features_;
};

struct Pose : public Message {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  Pose(const timestamp_t &ts, const SE3 &g) : Message{ts}, g_{g} {}

  SE3 g_;
};

} // namespace msg

} // namespace feh
