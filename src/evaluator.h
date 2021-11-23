#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <fstream>
#include <vector>

#include "buffer_manager.h"
#include "common.h"

class Evaluator {
 public:
  Evaluator(std::string trace_filepath, BufferManager* buffer_manager,
            int num_threads);
  ~Evaluator();
  void Evaluate();
  void EvaluateSingleThread(int rank);

 private:
  std::vector<std::pair<int, int>> trace_file_data_;
  BufferManager* buffer_manager_;
  int num_threads_;
  std::chrono::steady_clock::time_point begin_time_;
};

#endif  // EVALUATOR_H_