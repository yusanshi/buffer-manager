#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <fstream>

#include "buffer_manager.h"
#include "common.h"

class Evaluator {
 public:
  Evaluator(std::string trace_filepath, BufferManager* buffer_manager);
  void Evaluate();

 private:
  std::ifstream current_file_;
  BufferManager* buffer_manager_;
};

#endif  // EVALUATOR_H_