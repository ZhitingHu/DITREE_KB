
#include "ditree.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <petuum_ps_common/include/petuum_ps.hpp>

// Petuum Parameters
DEFINE_string(hostfile, "",
    "Path to file containing server ip:port.");
DEFINE_int32(num_clients, 1, 
    "Total number of clients");
DEFINE_int32(num_app_threads, 1, 
    "Number of app threads in this client");
DEFINE_int32(client_id, 0, 
    "Client ID");
DEFINE_string(consistency_model, "SSPPush", 
    "SSP or SSPPush");
DEFINE_string(stats_path, "", 
    "Statistics output file");
DEFINE_int32(num_comm_channels_per_client, 1,
    "number of comm channels per client");
DEFINE_int32(param_table_staleness, 0, 
    "staleness for weight tables.");
DEFINE_int32(loss_table_staleness, 5, 
    "staleness for loss tables.");
DEFINE_int32(row_oplog_type, petuum::RowOpLogType::kDenseRowOpLog,
    "row oplog type");
DEFINE_bool(oplog_dense_serialized, true, 
    "True to not squeeze out the 0's in dense oplog.");
DEFINE_int32(num_clocks_per_epoch, 10, 
    "Number of clocks per thread per epoch");

// PS Table Organization Paremeters
DEFINE_int32(max_depth, 10,
    "Maximum depth of a vertex.");
DEFINE_int32(max_num_children_per_vertex, 10,
    "Maximum number of children of a vertex.");
DEFINE_int32(max_num_vertexes, 10000,
    "Maximum number of children of a vertex.");
DEFINE_int32(max_size_per_table, 100,
    "Maximum number of parent of a table.");
DEFINE_int32(max_split_per_table, 1,
    "Maximum times of split each epoch per table.");
DEFINE_int32(max_merge_per_table, 0,
    "Maximum times of merge each epoch per table.");
DEFINE_int32(num_table_id_bits, 8,
    "Number of digit for representing table id, must be in (0, 32).");

// DITree Parameters
DEFINE_string(solver, "",
    "The solver definition protocol buffer text file.");
DEFINE_string(model, "",
    "The model definition protocol buffer text file.");
DEFINE_string(snapshot, "",
    "Optional; the snapshot solver state to resume training.");
DEFINE_string(params, "",
    "Optional; the model parameters to fine tuning.");
DEFINE_string(ditree_outputs, "",
    "The prefix of the ditree output file.");
DEFINE_int32(history_size, 1,
    "Number of history time slice to consider.");
DEFINE_string(history, "",
    "Optional; the model history.");

// Data Parameters
DEFINE_string(train_data, "",
    "The training data path.");
DEFINE_string(test_data, "",
    "The test data path.");
DEFINE_string(mean, "",
    "Mean of the word vectors.");
DEFINE_string(vocab, "",
    "Vocabulary file.");
DEFINE_int32(vocab_size, 0,
    "Size of the vocabulary.");
DEFINE_int32(train_batch_size, 100,
    "Size of a minibatch for training.");
DEFINE_int32(test_batch_size, 100,
    "Size of a minibatch for test.");

// KB Parameters
DEFINE_string(kb_data, "",
    "The knowledge base data path.");
DEFINE_double(kb_weight, 0.0,
    "The confidence level of the KB prior.");

//Other Parameters
DEFINE_int32(random_seed, -1,
    "Use system time as rand seed by default.");
DEFINE_int32(top_k, 10,
    "Number of top words of each topic to show");


int main(int argc, char** argv) {
  // Print output to stderr (while still logging).
  FLAGS_alsologtostderr = 1;
  // Do not buffer
  FLAGS_logbuflevel = -1;
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  CHECK_GT(FLAGS_solver.size(), 0) << "Need a solver definition to train.";
  CHECK_GT(FLAGS_model.size(), 0) << "Need a model definition to train.";

  // Initialize Context
  ditree::Context& context = ditree::Context::Get(); 
  context.Init();

  ditree::SolverParameter solver_param;
  ditree::ReadProtoFromTextFileOrDie(FLAGS_solver, &solver_param);
  ditree::DITreeEngine* ditree_engine = new ditree::DITreeEngine(solver_param);
  ditree_engine->ReadData();
  ditree_engine->ReadKB(ditree_engine->train_data().vocab());
  ditree_engine->Init();

  LOG(INFO) << "Starting NN with " << FLAGS_num_app_threads << " threads "
      << "on client " << FLAGS_client_id;
  
  std::vector<std::thread> threads(FLAGS_num_app_threads); 
  for (auto& thr : threads) {
    thr = std::thread(&ditree::DITreeEngine::Start, std::ref(*ditree_engine));
  }
  for (auto& thr : threads) {
    thr.join();
  }

  LOG(INFO) << "Optimization Done.";

  petuum::PSTableGroup::ShutDown();
  LOG(INFO) << "DITree finished and shut down!";

  return 0;
}
