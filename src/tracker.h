// The feature tracking module;
// Multi-scale Lucas-Kanade tracker from OpenCV.
// Author: Xiaohan Fei (feixh@cs.ucla.edu)
#pragma once

#include <list>
#include <memory>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "json/json.h"

#include "core.h"

namespace xivo {


/** Category of Optical Flow Algorithm for low-level feature tracking. */
enum class OpticalFlowType : int {
  LUCAS_KANADE = 0,
  FARNEBACK = 1
};


/** Lucas-Kanade Tracker Params */
typedef struct LKParams {
  int win_size;
  int max_level;
  int max_iter;
  number_t eps;
} LKParams;


/** Farneback optical flow params */
typedef struct FarnebackParams {
  int num_levels;
  number_t pyr_scale;
  bool use_fast_pyramids;
  int win_size;
  int num_iter;
  int polyN;
  number_t polySigma;
  int flags;
} FarnebackParams;


class Tracker {
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  static TrackerPtr Create(const Json::Value &cfg);
  static TrackerPtr instance() { return instance_.get(); }

  /** Matches features found on incoming image `img` to features in `features_`
   *  using LK-pyramid and detects a new set of features to be tracked.
   *  \todo Rescue features that would otherwise be dropped from tracker with newly
   *        detected features. */
  void Update(const cv::Mat &img);

public:
  std::list<FeaturePtr> features_;

private:
  Tracker(const Tracker &other) = delete;
  Tracker &operator=(const Tracker &other) = delete;

  Tracker(const Json::Value &cfg);
  static std::unique_ptr<Tracker> instance_;

  // Optical Flow Variant
  OpticalFlowType optflow_class_;

  // variables
  bool initialized_;
  Json::Value cfg_;
  int descriptor_distance_thresh_; // use this to verify feature tracking
  int max_pixel_displacement_;     // pixels shifted larger than this amount are
                                   // dropped

  cv::Mat img_;

  /** Last computed LK pyramid */
  std::vector<cv::Mat> pyramid_;

  /** Last computed Farneback Optical Flow */
  bool farneback_flow_initialized_;
  cv::Ptr<cv::Mat> farneback_flow_;

  /** Number of rows in the input image. */
  int rows_;
  /** Number of columns in the input image. */
  int cols_;

  // for the geneirc feature2d interface, see the following openc document:
  // https://docs.opencv.org/3.4/d0/d13/classcv_1_1Feature2D.html
  cv::Ptr<cv::Feature2D> detector_, extractor_;
  bool extract_descriptor_;

  /**
   * A "helper" grayscale image that indicates where the feature detector is
   * allowed to find features. Features are only valid in places where the mask
   * is white. (Pixels in `mask_` are black or white.) The dimensions are
   * `rows_-2*margin_` x `cols_-2*margin_`. The purpose of `mask_` is to prevent
   * too many features in the same location and to prevent features from being
   * detected at the very edges of images.
   */
  cv::Mat mask_;
  /** Number of pixels around a currently tracked feature where we shouldn't look
   *  for new features (so that we don't have two features for the same corner) */
  int mask_size_;
  int margin_;

  // LK optical flow params
  LKParams lk_params_;

  // Farneback Optical flow parameters
  FarnebackParams fb_params_;

  // fast params
  int num_features_min_;
  int num_features_max_;

  // Matching newly detected tracks to tracks that were just dropped by
  // the optical flow algorithm
  bool match_dropped_tracks_;
  std::vector<FeaturePtr> newly_dropped_tracks_;
  cv::Ptr<cv::BFMatcher> matcher_;

private:
  void InitializeTracker(const cv::Mat &image);
  void UpdateFarneback(const cv::Mat &image);
  void UpdatePyrLK(const cv::Mat &image);

  void Detect(const cv::Mat &img, int num_to_add);

  bool FindMatchInDroppedTracks(cv::Mat new_feature_descriptor,
    FeaturePtr *output_match);
};

// helpers

/** Called right before detecting a set of features on a new image. Makes all of
 *  `mask_` white. */
void ResetMask(cv::Mat mask);

/** Makes all the pixels in a `mask_size` x `mask_size` box centered at pixel `(x,y)`
 *  in `mask_` black. Called after each new detection is found. */
void MaskOut(cv::Mat mask, number_t x, number_t y, int mask_size = 15);

/** Checks whether or not `mask_` is white at pixel `(x,y)` and whether or not
 *  (x,y) is not too close to the edge of the image. */
bool MaskValid(const cv::Mat &mask, number_t x, number_t y);

} // namespace xivo
