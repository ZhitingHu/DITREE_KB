#ifndef DITREE_SOLVER_HPP_
#define DITREE_SOLVER_HPP_

#include "common.hpp"
#include "context.hpp"
#include "tree.hpp"
#include "dataset.hpp"
#include "target_dataset.hpp"

namespace ditree {

class Solver {
 public:
  explicit Solver(const SolverParameter& param, const int thread_id,
      Dataset* train_data, Dataset* test_data);
  explicit Solver(const string& param_file, const int thread_id,
      Dataset* train_data, Dataset* test_data);

  void Solve(const char* resume_file = NULL);
  inline void Solve(const string resume_file) { Solve(resume_file.c_str()); }
 
 private: 
  void Init(const SolverParameter& param);
  void Test();
  void UpdateTestLossTable(const float log_likelihood, const int num_datum);
  void Update();

  float AddKBConstraints(const UIntFloatMap& data, const UIntFloatMap& vertex_kb_w);

  // Split move
  void ClearLastSplit();
  void SampleVertexToSplit();
  void CollectTargetData(const UIntFloatMap& log_weights,
      const float log_weight_sum, const Datum* datum);
  void Split();
  void SplitInit(Vertex* parent, Vertex* new_child, TargetDataset* target_data);
  void RestrictedUpdate(Vertex* parent, Vertex* new_child,
      TargetDataset* target_data);
  
  // Merge move
  void Merge();
  void ConstructMergeMap();
  void FinishMergeMove();

  void Snapshot();
  void Restore(const char* resume_file);

  void RegisterPSTables();
 private:
  SolverParameter param_;

  int client_id_;
  int thread_id_;

  Tree* tree_;
  Vertex* root_;

  Dataset* train_data_;
  Dataset* test_data_;
  int epoch_;
  int iter_;

  // KB
  float kb_weight_;

  // for split
  vector<uint32> vertexes_to_split_;
  bool collect_target_data_;
  float log_target_data_threshold_;
  int max_target_data_size_;
  // parent_idx => target data
  map<uint32, TargetDataset*> split_target_data_;
  // parent_idx => target data
  map<uint32, TargetDataset*> split_target_data_last_;

  // for merge
  vector<pair<uint32, uint32> > vertex_pairs_to_merge_;
  // guest_idx => host_idx of the last merge move
  map<uint32, uint32> merged_vertexes_host_idx_;
  
  //petuum::Table<float> train_loss_table_;
  petuum::Table<float> test_loss_table_;
  int display_counter_;
  int test_counter_;
  // print results on the fly
  int display_gap_;
  int test_display_gap_;
  petuum::HighResolutionTimer total_timer_;
  
};

} // namespace ditree

#endif
