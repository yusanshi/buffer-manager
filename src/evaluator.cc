#include "evaluator.h"

#include <iostream>

Evaluator::Evaluator(std::string trace_filepath,
                     BufferManager* buffer_manager) {
  this->trace_file_.open(trace_filepath);
  if (!this->trace_file_.is_open()) {
    throw std::runtime_error("Failed to open trace file.");
  }

  this->buffer_manager_ = buffer_manager;
}

void Evaluator::Evaluate() {
  std::cout << "Running evaluation..." << std::endl;
  int mode, page_id;
  char delimiter;
  while ((this->trace_file_ >> mode >> delimiter >> page_id) &&
         (delimiter == ',')) {
    page_id -= 1;
    this->buffer_manager_->FixPage(page_id);
    if (mode == 0) {
      Page data = this->buffer_manager_->ReadPage(page_id);
    } else if (mode == 1) {
      Page data;
      this->buffer_manager_->WritePage(page_id, data);
    } else {
      throw std::runtime_error("Bad trace format.");
    }
    this->buffer_manager_->UnfixPage(page_id);
  }
}
