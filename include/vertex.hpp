#ifndef DITREE_VERTEX_HPP_
#define DITREE_VERTEX_HPP_

#include "common.hpp"
#include "proto/ditree.pb.h"

namespace ditree {

class Vertex {
 public:
  explicit Vertex() { Init(); }
  explicit Vertex(const VertexParameter& param,
      const TreeParameter& tree_param); 
  void Init();
  void LoadHistory(const VertexParameter& history);

  void RecursConstructParam();
  void ConstructParam();
  void EstimateBeta();
  void RecursComputeVarZPrior();
  void ComputeVarZPrior();

  void RecursComputeKBWeight();
  void ComputeKBWeight();

  void InitParam(const float n_init, const FloatVec& s_init);
  void InitParamFromHistoryAndParent();
  void FromProto();

  void ReadParamTable();
  // Overload update functions
  void UpdateParamTable(const float data_batch_n_z_new, 
    const float data_batch_n_z_old, const UIntUIntMap& word_idxes,
    const FloatVec& data_batch_s_z_new, const FloatVec& data_batch_s_z_old);
  void UpdateParamTable(const float data_batch_n_z_new,
      const float data_batch_n_z_old, const FloatVec& data_batch_s_z_new,
      const FloatVec& data_batch_s_z_old);
  void UpdateParamTableByInc(const float n_z, const UIntUIntMap& word_idxes,
      const FloatVec& s_z, const float coeff);
  void UpdateParamTableByInc(const float n_z, const FloatVec& s_z,
      const float coeff);

  float ComputeELBO() const;

  void UpdateParamLocal(const float n_z_new, const float n_z_old,
    const UIntUIntMap& word_idxes, const FloatVec& s_z_new, 
    const FloatVec& s_z_old);
  
  void RecursPrintChildrenList(ostringstream& oss) const;
  void RecursPrintTopWords(const map<int, string>& vocab) const;
  void PrintTopWords(const map<int, string>& vocab) const;
  int CalSubTreeSize() const;
  
  void ToProto(VertexParameter* param);

  inline FloatVec& mutable_mean() { return mean_; }
  inline const FloatVec& mean() const { return mean_; }
  inline const FloatVec& mean_history() const { 
#ifdef DEBUG
    CHECK_EQ(new_born_, false);
#endif
    return mean_history_; 
  }

  inline float kappa() const { return kappa_; }
  inline float beta() const { return beta_; }
  inline float kappa_0() const { return kappa_0_; }
  inline float kappa_1() const { return kappa_1_; }
  inline float kappa_2() const { return kappa_2_; }
  inline float alpha() const { return alpha_; }
  inline float gamma() const { return gamma_; }

  inline float tau(const int idx) const { 
#ifdef DEBUG
    CHECK_LT(idx, 2);
#endif
    return tau_[idx]; 
  }
  inline float sigma(const int idx) const { 
#ifdef DEBUG
    CHECK_LT(idx, 2);
#endif
    return sigma_[idx]; 
  }
  inline bool new_born() const {
    return new_born_;
  }

  inline float var_n_sum_for_parent() const {
    return var_n_sum_for_parent_;
  }
  inline float var_n_sum_for_sibling() const {
    return var_n_sum_for_sibling_;
  }
  inline float var_z_prior_part_for_child() const {
    return var_z_prior_part_for_child_; 
  }
  inline float var_z_prior_part_for_sibling() const {
    return var_z_prior_part_for_sibling_; 
  }
  inline float var_z_prior() const { return var_z_prior_; }

  inline const UIntFloatMap& kb_word_reg_weight() const {
    return kb_word_reg_weight_;
  }
  inline const UIntFloatMap& kb_word_reg_weight_for_child() const {
    return kb_word_reg_weight_for_child_;
  }

  inline uint32 idx() const { return idx_; }
  inline void set_idx(const uint32 idx) { idx_ = idx; }
  inline uint32 table_idx() const { return table_idx_; }
  inline void set_table_idx(const uint32 table_idx) { table_idx_ = table_idx; }

  inline float n() const { return n_; }
  inline float& mutable_n() { return n_; }
  inline float temp_n() const { return temp_n_; }
  inline float& mutable_temp_n() { return temp_n_; }
  inline const FloatVec& s() const { return s_; }
  inline FloatVec& mutable_s() { return s_; }

  inline const vector<Vertex*>& children() const {
    return children_;
  }
  inline void add_child(Vertex* child) {
    add_temp_child(child);
    if(child->left_sibling() != NULL) {
      child->left_sibling()->set_right_sibling(child);
    }
  }
  // Add temp child, do not set the right_sibling of the 
  //   existing right-most child
  inline void add_temp_child(Vertex* child) {
    child->set_parent(this);
    child->set_depth(depth_ + 1);
    if (children_.size() > 0) {
      Vertex* child_left_sibling = children_[children_.size() - 1];
      child->set_left_sibling(child_left_sibling);
    }
    children_.push_back(child);
  }
  inline void remove_child(Vertex* child) {
    if (child->left_sibling() != NULL) {
      child->left_sibling()->set_right_sibling(child->right_sibling());
    }
    if (child->right_sibling() != NULL) {
      child->right_sibling()->set_left_sibling(child->left_sibling());
    }
    const int ori_child_size = children_.size();
    for (int pos = 0; pos < children_.size(); ++pos) {
      if (children_[pos]->idx() == child->idx()) {
        children_.erase(children_.begin() + pos);
        break;
      }
    }
    CHECK_EQ(ori_child_size, children_.size() + 1);
  }

  inline Vertex* parent() const { return parent_; }
  inline void set_parent(Vertex* parent) { parent_ = parent; }
  inline Vertex* left_sibling() const { return left_sibling_; }
  inline void set_left_sibling(Vertex* left_sibling) { 
    left_sibling_ = left_sibling; 
  }
  inline Vertex* right_sibling() const { return right_sibling_; }
  inline void set_right_sibling(Vertex* right_sibling) { 
    right_sibling_ = right_sibling; 
  }
  inline uint32 child_table_idx() const { 
    return child_table_idx_; 
  }
  inline void set_child_table_idx(const uint32 child_table_idx) { 
    child_table_idx_ = child_table_idx; 
  }
  inline bool root() const { return root_; }
  inline void set_root() { root_ = true; }
  inline int depth() { return depth_; }
  inline void set_depth(const int depth) { depth_ = depth; }
  void RecursSetDepth(const int parent_depth);

  void CopyParamsFrom(const Vertex* source);
  void MergeFrom(const Vertex* host, const Vertex* guest);

 private:
  
  inline float ComputeTaylorApprxCoeff(const float rho_apprx);

 private:
  //VertexParameter vertex_param_;

  /// tree structure
  uint32 idx_; // idx_ will not change over life
  uint32 table_idx_; // table_idx_ can be changed, e.g., by table merging 
  Vertex* parent_;
  Vertex* left_sibling_;
  Vertex* right_sibling_;
  vector<Vertex*> children_;
  // a vextex's children are guaranteed to be in the same table
  uint32 child_table_idx_;
  // root node has depth=1, psi=1
  bool root_;
  // depth of this node (root has depth 1)
  int depth_;

  /// (global) parameters
  // emission ~ vMF(mean_, kappa_)
  FloatVec mean_;
  float kappa_;
  // prior
  float tau_[2]; // prior of \nu
  float sigma_[2]; // prior of \psi

  // (local)
  float var_n_sum_for_parent_; // \sum_{z <= z.i} n_(z.i)
  float var_n_sum_for_sibling_; //\sum_{j >= i}{z.j <= z.j.k} n_{z.j.k} 

  /// (local) history
  float tau_fixed_part_[2]; // 1 + history, \alpha + history
  float sigma_fixed_part_[2]; // 1 + history, \gamma + history
  // f(z) = \sum_{t'}{n} < q(z = z_n), q(z < z_n) >
  FloatVec tau_history_[2]; 
  // f(z.i) = \sum_{t'}{n} < q(z.i <= z_n), \sum_{j} q(z.j <= z_n) >
  FloatVec sigma_history_[2];
  // mean at time stamp (t-1)
  FloatVec mean_history_;
  bool new_born_;

  /// (global) memoized VI
  float n_; // \sum_{n} q(z_n = z)
  FloatVec s_; // \sum_{n} q(z_n = z) * x_n
  //float h_; // \sum_{n} q(z_n = z) * log q(z_n = z)

  // a temp variable used in split
  float temp_n_;

  /// (local) VI
  // \sum_{z' <= z} E[ log (1 - \nu_z') ] + E[ log \phi_z' ]
  float var_z_prior_part_for_child_;
  // \sum_{j <= i} E[ log (1 - \psi_{j}) ]
  float var_z_prior_part_for_sibling_;
  // E[ log p(z | \nu, \psi) ]
  float var_z_prior_;
  
  /// (local) KB posterior regularization
  // word => sum of weights of the word's KB-children-words in the vertex's ascendants
  UIntFloatMap kb_word_reg_weight_;
  UIntFloatMap kb_word_reg_weight_for_child_;

  /// fixed parameters
  float kappa_0_;
  float kappa_1_;
  float kappa_2_;
  float beta_;
  float alpha_;
  float gamma_;
};

} // namespace ditree

#endif
