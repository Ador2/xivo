// Bundle Adjustment/Pose Graph Optimization module.
// Run in the background, trigger every ... seconds?
// Author: Xiaohan Fei (feixh@cs.ucla.edu)
#pragma once
#include <memory>

// g2o
#include "g2o/config.h"
#include "g2o/core/sparse_optimizer.h"
#include "g2o/core/block_solver.h"
#include "g2o/core/solver.h"
#include "g2o/core/robust_kernel_impl.h"
#include "g2o/core/optimization_algorithm_levenberg.h"
#include "g2o/solvers/dense/linear_solver_dense.h"
#include "g2o/types/icp/types_icp.h"
#include "g2o/solvers/structure_only/structure_only_solver.h"

#include "g2o/solvers/cholmod/linear_solver_cholmod.h"
#include "g2o/solvers/csparse/linear_solver_csparse.h"

#include "core.h"

namespace feh {

class Optimizer;
using OptimizerPtr = Optimizer*;

class Optimizer {
public:
  ~Optimizer();
  static OptimizerPtr Create(const Config &cfg);
  static OptimizerPtr instance();
  void Solve(int iters=1);

private:
  Optimizer() = delete;
  Optimizer(const Optimizer &) = delete;
  Optimizer &operator=(const MemoryManager &) = delete;

  Optimizer(const Config &cfg);

  // the instance class memeber
  static std::unique_ptr<Optimizer> instance_;

  // flags

  // g2o variables
  g2o::SparseOptimizer optimizer_;
  std::unique_ptr<g2o::BlockSolver_6_3::LinearSolverType> solver_;
  std::unique_ptr<g2o::OptimizationAlgorithmLevenberg> algorithm_;
};

} // namespace feh
