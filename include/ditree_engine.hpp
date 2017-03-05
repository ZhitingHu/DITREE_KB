#ifndef DITREE_DITREE_ENGINE_HPP_
#define DITREE_DITREE_ENGINE_HPP_

#include "common.hpp"
#include "context.hpp"
#include "dataset.hpp"
#include <atomic>

namespace ditree {

class DITreeEngine {
 public:
  explicit DITreeEngine(const SolverParameter& param);
  
  void Init();

  void ReadData();

  void ReadKB(const map<int, string>& vocab);

  // Can be called concurrently.  
  void Start();

  inline Dataset& train_data() { return train_data_; } 

 private:
  void CreateTables();

 private:
  SolverParameter solver_param_;

  Dataset train_data_;
  Dataset test_data_;
  
  // parent word => child word
  UIntVecUIntMap kb_;

  // 
  std::atomic<int> thread_counter_;
};

} // namespace ditree

#endif
